//
// Created by miro on 12/6/16.
//

#include <iostream>
#include <gtest/gtest.h>
#include <memory>

#include <befa/utils/visitor.hpp>
#include <befa/assembly/instruction_parser.hpp>
#include <befa/assembly/instruction.hpp>
#include <befa/llvm/call.hpp>
#include <befa/llvm/cmp.hpp>
#include <befa.hpp>

namespace {
struct dummy_parent {};

using Instruction = ExecutableFile::inst_t::info::type;
using Symbol = ExecutableFile::sym_t::info::type;
using SymbolTableMap = ExecutableFile::map_t::info::type;

struct InstructionTemplate
    : public Instruction {
  InstructionTemplate(const string &decoded)
      : Instruction({}, bb_t::ptr::weak(), decoded, 0x666) {}
};

struct DummySymbol
    : public Symbol {
  DummySymbol(std::string name, bfd_vma address)
      : Symbol(nullptr, nullptr), name(name), address(address) {}

  string getName() const override {
    return name;
  }

  bfd_vma getAddress() const override {
    return address;
  }

 private:
  std::string name;
  bfd_vma address;
};

void test_single_instruction(
    std::vector<std::string> signatures,
    SymbolTableMap symbol_map,
    std::string compare
) {
  // instruction created from signature only (dummy)
  auto instruction_stream = rxcpp::sources::iterate(signatures)
      // Don't care about copy-loss cast, InstructionTemplate doesn't have data
      .map([](std::string signature) -> ExecutableFile::inst_t::info::type {
        return InstructionTemplate(signature);
      });

  // to create llvm instruction we need factory
  auto symbol_table =
      std::make_shared<llvm::SymTable>(std::shared_ptr<SymbolTableMap>(
          &symbol_map, [](void *) {}
      ));

  // subj of instruction stream (via this you can send instructions)
  rxcpp::subjects::subject<Instruction> i_subj;

  // create mapper (contains obs - as_observable())
  std::shared_ptr<llvm::InstructionMapper> mapper
      = std::make_shared<llvm::InstructionMapper>(symbol_table);

  // append Call factory
  mapper->register_factory(
      std::make_shared<llvm::CallFactory>()
  );
  mapper->register_factory(
      std::make_shared<llvm::CompareFactory>()
  );
  bool reduced = false;
  // hook instruction mapper into instruction stream
  auto sym_table_subsc = mapper
      ->reduce_instr(i_subj.get_observable())
      .subscribe([&reduced](
          llvm::InstructionMapper::sym_table_t::ptr::shared ptr
      ) { reduced = true; });

  // send instruction
  instruction_stream.subscribe([&](const Instruction &simple_i) {
    // hook to stream of translated instructions
    auto subscription = mapper
        // get obs from mapper
        ->observable()
            // subscribe for newly generated instructions
        .subscribe([&](const std::shared_ptr<llvm::VisitableBase> &i) {
          // retrieve type of instruction
          invoke_accept(i, llvm::SerializableVisitorL([&](
              const llvm::Serializable *instr
          ) -> void { EXPECT_EQ(compare, instr->getSignature()); }));
        });

    auto subscriber = i_subj.get_subscriber();
    subscriber.on_next(simple_i);
    i_subj.get_observable()
          .subscribe([&reduced](const auto &) {
            EXPECT_TRUE(reduced);
          });
    subscriber.on_completed();
  });

//  mapper->reduce_instr(instruction_stream)
//        .subscribe([](llvm::InstructionMapper::sym_table_t::ptr::shared ptr) {
//
//        });
}

SymbolTableMap concat(SymbolTableMap a, SymbolTableMap b) {
  a.insert(b.cbegin(), b.cend());
  return std::move(a);
}



TEST(DecompilerTest, CallTest) {
  test_single_instruction(
      // asm instructions
      {"call    400800"},
      // symbol table
      concat(
          {
              {"400800",
               std::make_shared<symbol_table::Function>(
                   std::make_shared<DummySymbol>(
                       "printf",
                       0x400800
                   )
               )
              }
          },
          ::map(symbol_table::registers, [](
              std::pair<std::string, symbol_table::VisitableBase *> _reg
          ) {
            return std::make_pair(_reg.first, std::shared_ptr<
                symbol_table::VisitableBase
            >(
                _reg.second,
                symbol_table::register_deleter
            ));
          }, SymbolTableMap())
      ),
  // guessed result
      "call @printf()"
  );
}

TEST(DecompilerTest, CmpTest) {
  test_single_instruction(
      // asm instructions
      {"cmp    eax, ebx"},
      { // symbol table
          {"0",
           std::make_shared<
               symbol_table::Register<
                   symbol_table::types::DWORD
               >
           >("_eax")
          },
          {"1",
           std::make_shared<
               symbol_table::Register<
                   symbol_table::types::DWORD
               >
           >("_ebx")
          }
      },
      // guessed result
      "call @printf()"
  );

}
}  // namespace

//
//TEST(DecompilerTest, JumpTest) {
//  test_single_instruction(
//      "jmp    400800",
//      {
//          {0x400800,
//           std::make_shared<symbol_table::Function>(
//               std::make_shared<DummySymbol>(
//                   "printf",
//                   0x400800
//               )
//           )
//          }
//      },
//      "jmp @printf"
//  );
//  test_single_instruction(
//      "jmp    666",
//      {},
//      "jmp 0x666"
//  );
//}
