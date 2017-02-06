//
// Created by miro on 12/6/16.
//

#include <iostream>
#include <gtest/gtest.h>
#include <memory>

#include <befa/utils/visitor.hpp>
#include <befa/assembly/instruction_parser.hpp>
#include <befa/assembly/instruction.hpp>

// requirement classes
#include <befa/llvm/assignment.hpp>
#include <befa/llvm/unary_instruction.hpp>
#include <befa/llvm/jmp.hpp>
#include <befa/llvm/binary_operation.hpp>

// endpoint classes
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
    std::vector<std::string> compares
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
  mapper->register_factories(
      std::make_shared<llvm::CallFactory>(),
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
    int counter = 0;
    // hook to stream of translated instructions
    auto subscription = mapper
        // get obs from mapper
        ->observable()
            // subscribe for newly generated instructions
        .subscribe([&](const std::shared_ptr<llvm::VisitableBase> &i) {
          // retrieve type of instruction
          invoke_accept(i, llvm::SerializableVisitorL([&](
              const llvm::Serializable *instr
          ) -> void {
            ASSERT_LT(counter, compares.size());
            EXPECT_STREQ(compares[counter++].c_str(), instr->toString().c_str());
          }));
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
  static auto _symbol_table = ::map(symbol_table::registers, [](
      std::pair<std::string, symbol_table::VisitableBase *> _reg
  ) {
    return std::make_pair(
        _reg.first,
        std::shared_ptr<symbol_table::VisitableBase>(
            _reg.second,
            symbol_table::register_deleter
        ));
  }, SymbolTableMap());
  test_single_instruction(
      // asm instructions
      {"call    0x400800"},
      // symbol table
      concat(
          {
              {"printf",
               std::make_shared<symbol_table::Function>(
                   std::make_shared<DummySymbol>(
                       "printf",
                       0x400800
                   )
               )
              }
          },
          _symbol_table
      ),
  // guessed result
      {"ResultOfTheCall = call @printf()"}
  );
}

TEST(DecompilerTest, CmpTest) {
  static auto _symbol_table = ::map(symbol_table::registers, [](
      std::pair<std::string, symbol_table::VisitableBase *> _reg
  ) {
    return std::make_pair(
        _reg.first,
        std::shared_ptr<symbol_table::VisitableBase>(
            _reg.second,
            symbol_table::register_deleter
        ));
  }, SymbolTableMap());
  test_single_instruction(
      // asm instructions
      {
          "cmp    eax, ebx",
          "test   eax, ebx"
      },
      _symbol_table,
      // guessed result
      {"((BIT)_zf) = icmp eq ((DWORD)_eax), ((DWORD)_ebx)",
       "((BIT)_cf) = icmp lt ((DWORD)_eax), ((DWORD)_ebx)"}
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
