//
// Created by miro on 12/6/16.
//

#include <iostream>
#include <gtest/gtest.h>
#include <memory>

#include <befa/utils/visitor.hpp>
#include <befa/assembly/asm_arg_parser.hpp>
#include <befa/assembly/instruction.hpp>
#include <befa.hpp>
#include <befa/llvm/unary.hpp>

namespace {
struct dummy_parent {};

struct NonPreparsedInstruction
    : public ExecutableFile::instruction_type {
  NonPreparsedInstruction(const string &decoded)
      : ExecutableFile::instruction_type({}, nullptr, decoded, 0x666) {}

};

struct TestVisitor
    : public llvm::VisitorBase {

  std::string param_name;

  TestVisitor(std::string param_name) : param_name(param_name) {}

  LLVM_VISIT_ALL(arg) {
    ASSERT_EQ(arg->getSignature(), param_name);
  }
};

struct DummySymbol : public ExecutableFile::symbol_type {
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
    std::string signature,
    ExecutableFile::symbol_map_type symbol_map,
    std::string compare
) {
  NonPreparsedInstruction simple_instr(signature);
  llvm::InstructionFactory factory;
  ExecutableFile::symbol_map_type symbol_table{
      symbol_map
  };
  llvm::mappers::InstructionMapper<llvm::CallInstruction> mapper
      (factory, symbol_table);
  mapper(simple_instr);
  TestVisitor visitor(compare);
  for (auto &vis: factory.collect()) {
    vis->accept(visitor);
  }
}

TEST(DecompilerTest, CallTest) {
  test_single_instruction(
      "call    0x666",
      {
          {0x665,
           std::make_shared<symbol_table::Function>(
               std::make_shared<DummySymbol>(
                   "sample_callee_untrue",
                   0x665
               )
           )
          },
          {0x666,
           std::make_shared<symbol_table::Function>(
               std::make_shared<DummySymbol>(
                   "sample_callee",
                   0x666
               )
           )
          },
          {0x667,
           std::make_shared<symbol_table::Function>(
               std::make_shared<DummySymbol>(
                   "false",
                   0x667
               )
           )
          }
      },
      "call @sample_callee()"
  );
}
/**
 *

 */
}  // namespace