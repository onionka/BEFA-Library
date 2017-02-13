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
using SymbolMap = ExecutableFile::map_t::info::type;
using SymbolTable = llvm::InstructionMapper::sym_table_t::type;

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

using namespace rxcpp::operators;

void test_asm_to_llvm(
    std::vector<std::string> signatures,
    SymbolMap symbol_map,
    std::vector<std::string> compares
) {
  // instructions created from signatures only (dummy)
  auto instruction_stream = rxcpp::sources::iterate(signatures)
      // Don't care about slicing, InstructionTemplate doesn't have any data
      .map([](std::string signature) -> ExecutableFile::inst_t::type {
        return InstructionTemplate(signature);
      });

  // to create llvm instruction we need symbol table object (not a map)
  auto symbol_table =
      std::make_shared<llvm::SymTable>(
          // shared_ptr on stack (not heap)
          // still only exists in this function
          // no sigsegv possible
          std::shared_ptr<SymbolMap>(
              &symbol_map, [](void *) {}
          )
      );

  // subj of instruction stream (via this you can send instructions)
  rxcpp::subjects::subject<Instruction> i_subj;

  // create mapper (contains obs - as_observable())
  std::shared_ptr<llvm::InstructionMapper> mapper
      = std::make_shared<llvm::InstructionMapper>(symbol_table);

  // append Call factory
  mapper->register_factories(
      std::make_shared<llvm::CallFactory>(),
      std::make_shared<llvm::CompareFactory>(),
      std::make_shared<llvm::JumpFactory>()
  );
  // result of reduction will be stored here
  // reduction is a synchronous operation
  std::shared_ptr<SymbolTable> sym_table_ptr;
  // hook instruction mapper into instruction stream
  auto sym_table_subsc = mapper
      ->reduce_instr(i_subj.get_observable())
          // its just one
      .first()
      .subscribe([&](
          std::shared_ptr<SymbolTable> ptr
      ) { sym_table_ptr = ptr; });

  // hook to stream of translated instructions
  auto subscription = (
      // zip llvm instructions with comparision string vector
      rxcpp::sources::iterate(compares) | zip(
      [](
          std::string compare,
          std::shared_ptr<llvm::VisitableBase> instr
      ) -> bool {
        EXPECT_STREQ(
            compare.c_str(),
            // convert instruction to string
            map_visitable<llvm::SerializableVisitorL>(
                instr,
                [](const llvm::Serializable *i) {
                  return i->toString();
                }
            ).c_str()
        );
        return true;
      },
      mapper->observable()
  ))
      // subscribe for newly generated symbol table
      .subscribe([](bool) { });

  auto subscriber = i_subj.get_subscriber();

  // send instruction
  instruction_stream.subscribe([&](const Instruction &simple_i) {
    subscriber.on_next(simple_i);
  });
  // remember subscription
  auto subs = i_subj
      .get_observable()
      .subscribe([&sym_table_ptr](const auto &) {
        EXPECT_TRUE((bool) sym_table_ptr);
      });
  subscriber.on_completed();
  subs.unsubscribe();
}

SymbolMap concat(SymbolMap a, SymbolMap b) {
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
  }, SymbolMap());
  test_asm_to_llvm(
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
  }, SymbolMap());

  test_asm_to_llvm(
      // asm instructions
      {
          "test   eax, ebx",
          "cmp    eax, ebx"
      },
      _symbol_table,
      // guessed result
      {
          // test
          "Temporary = (((DWORD)_eax)) AND (((DWORD)_ebx))",
          "((BIT)_sf) = MSB(Temporary)",
          "((BIT)_zf) = icmp eq Temporary, 0",
          "((BIT)_pf) = BitwiseXNOR(Temporary)",
          // cmp
          "((BIT)_zf) = icmp eq ((DWORD)_eax), ((DWORD)_ebx)",
          "((BIT)_cf) = icmp lt ((DWORD)_eax), ((DWORD)_ebx)",
      }
  );
}

TEST(DecompilerTest, JmpTest) {
  static auto _symbol_table = ::map(symbol_table::registers, [](
      std::pair<std::string, symbol_table::VisitableBase *> _reg
  ) {
    return std::make_pair(
        _reg.first,
        std::shared_ptr<symbol_table::VisitableBase>(
            _reg.second,
            symbol_table::register_deleter
        ));
  }, SymbolMap());

  test_asm_to_llvm(
      // asm instructions
      {
          "cmp    eax, ebx",
          "jbe    0x0"
      },
      _symbol_table,
      // guessed result
      {
          // cmp
          "((BIT)_zf) = icmp eq ((DWORD)_eax), ((DWORD)_ebx)",
          "((BIT)_cf) = icmp lt ((DWORD)_eax), ((DWORD)_ebx)",
          // jbe
          "TempResult = icmp eq ((BIT)_cf), 1",
          "Temp2Result = icmp eq ((BIT)_zf), 1",
          "TempResult = (Temp2Result) OR (TempResult)",
          "br TempResult, address 0"
      }
  );
}
}  // namespace

