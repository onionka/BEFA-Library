//
// Created by miro on 12/3/16.
//

#include <iostream>
#include <gtest/gtest.h>
#include <memory>

#define INSTRUCTION_TEST

#include <befa/utils/visitor.hpp>
#include <befa/assembly/asm_arg_parser.hpp>
#include <befa/assembly/instruction.hpp>
#include <befa.hpp>

namespace {

struct dummy_parent { };

struct SimpleInstruction
    : public befa::Instruction<dummy_parent> {
  SimpleInstruction(const instruction_pieces &pieces)
      : pieces(pieces) {}

  instruction_pieces parse() const override {
    return pieces;
  }

 private:
  instruction_pieces pieces;
};

struct NonPreparsedInstruction
    : public befa::Instruction<dummy_parent> {
  NonPreparsedInstruction(const string &decoded)
      : befa::Instruction<dummy_parent>({}, nullptr, decoded, 0x666) {}
};

TEST(DecoderTest, BasicInstruction) {
  SimpleInstruction simple_instr({"mov", "eax", "ebx"});
  simple_instr.getArgs();
}

struct TestVisitor
    : public symbol_table::VisitorBase {

  std::string param_name;

  TestVisitor(std::string param_name) : param_name(param_name) {}

  ASM_VISIT_ALL(arg) {
    ASSERT_EQ(arg->getName(), param_name);
  }
};

TEST(DecoderTest, WithoutPreparseInstruction) {
  NonPreparsedInstruction simple_instr("mov eax,ebx");
  auto args = simple_instr.getArgs();
  ASSERT_EQ(args.size(), 2);
  TestVisitor visitor("_eax");
  args[0]->accept(visitor);
  visitor.param_name = "_ebx";
  args[1]->accept(visitor);
}

TEST(DecoderTest, TestDereference) {
  NonPreparsedInstruction simple_instr("mov DWORD PTR [eax*0x8+0x666],ebx");
  auto args = simple_instr.getArgs();
  ASSERT_EQ(args.size(), 2);
  TestVisitor visitor("*(_eax*0x8+0x666)");
  args[0]->accept(visitor);
  visitor.param_name = "_ebx";
  args[1]->accept(visitor);
}

TEST(DecoderTest, TestXMM) {
  NonPreparsedInstruction simple_instr("mov XMMWORD PTR [eax*0x8+0x666],ebx");
  auto args = simple_instr.getArgs();
  ASSERT_EQ(args.size(), 2);
  TestVisitor visitor("*(_eax*0x8+0x666)");
  args[0]->accept(visitor);
  visitor.param_name = "_ebx";
  args[1]->accept(visitor);
}

TEST(DecoderTest, TestLEA) {
  NonPreparsedInstruction simple_instr("jne    4009b0");
  auto args = simple_instr.getArgs();
  ASSERT_EQ(args.size(), 1);
  TestVisitor visitor("4009b0");
  args[0]->accept(visitor);
}

struct DummySymbol : public ExecutableFile::symbol_type  {
  DummySymbol()  : Symbol(nullptr, null_section) {}

  string getName() const override {
    return "number_of_the_beast";
  }

  std::shared_ptr<befa::Section> null_section = std::make_shared<befa::Section>(nullptr);
};

TEST(DecoderTest, TestFunctionFeed) {
  auto dummy_function = std::make_shared<DummySymbol>();
  NonPreparsedInstruction simple_instr("call    0x666");
  std::map<
      bfd_vma, std::shared_ptr<symbol_table::VisitableBase>
  > sym_table {
      std::make_pair(0x666, std::make_shared<symbol_table::Function>(
          dummy_function
      ))
  };
  auto args = simple_instr.getArgs(sym_table);
  ASSERT_EQ(args.size(), 1);
  TestVisitor visitor("@number_of_the_beast");
  args[0]->accept(visitor);
}
}
