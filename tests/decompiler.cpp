//
// Created by miro on 12/6/16.
//

#include <iostream>
#include <gtest/gtest.h>
#include <memory>

#include <befa/utils/visitor.hpp>
#include <befa/assembly/asm_arg_parser.hpp>
#include <befa/assembly/instruction.hpp>
#include <befa/llvm/call.hpp>
#include <befa/llvm/cmp.hpp>
#include <befa.hpp>


namespace {
struct dummy_parent {};

struct InstructionTemplate
    : public ExecutableFile::instruction_type {
  InstructionTemplate(const string &decoded)
      : ExecutableFile::instruction_type({}, nullptr, decoded, 0x666) {}
};

template<typename LambdaT>
struct TestVisitor : public llvm::VisitorBase {

  TestVisitor(LambdaT &&check)
      : check(std::forward<LambdaT>(check)) {}

  LLVM_VISIT_ALL(arg) {
    check(arg);
  }

 private:
  LambdaT check;
};

template<typename LambdaT>
TestVisitor<LambdaT> create_TestVisitor(LambdaT &&func) {
  return TestVisitor<LambdaT>(std::forward<LambdaT>(func));
}

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
  InstructionTemplate simple_instr(signature);

  llvm::InstructionFactory factory;
  ExecutableFile::symbol_map_type symbol_table{
      symbol_map
  };

  llvm::mappers::InstructionMapper<llvm::CallInstruction> mapper
      (factory, symbol_table);

  mapper(simple_instr);
  auto visitor = create_TestVisitor([&compare] (auto *instr) {
    ASSERT_EQ(instr->getSignature(), compare);
  });
  auto instr_vector = factory.collect();
  ASSERT_GT(instr_vector.size(), 0);
  for (auto &vis: instr_vector) {
    vis->accept(visitor);
  }
}

TEST(DecompilerTest, CallTest) {
  test_single_instruction(
      "call    400800",
      {
          {0x400800,
           std::make_shared<symbol_table::Function>(
               std::make_shared<DummySymbol>(
                   "printf",
                   0x400800
               )
           )
          }
      },
      "call @printf()"
  );
}
}  // namespace