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
#include <befa/llvm/instruction.hpp>

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

struct InstructionTemplate
    : public befa::Instruction<dummy_parent> {
  InstructionTemplate(const string &decoded)
      : befa::Instruction<dummy_parent>({}, nullptr, decoded, 0x666) {}
};

TEST(DecoderTest, BasicInstruction) {
  SimpleInstruction simple_instr({"mov", "eax", "ebx"});
  simple_instr.getArgs();
}


symbol_table::SymbolVisitorL create_test_visitor(std::string cmp) {
  return symbol_table::SymbolVisitorL(
      [&cmp](const symbol_table::Symbol *ptr) {
        ASSERT_EQ(ptr->getName(), cmp);
      }
  );
}

TEST(DecoderTest, WithoutPreparseInstruction) {
  InstructionTemplate simple_instr("mov eax,ebx");
  auto args = simple_instr.getArgs();
  ASSERT_EQ(args.size(), 2);
  invoke_accept(args[0], create_test_visitor("<DWORD> _eax"));
  invoke_accept(args[1], create_test_visitor("<DWORD> _ebx"));
}

TEST(DecoderTest, TestDereference) {
  InstructionTemplate simple_instr("mov DWORD PTR [eax*0x8+0x666],ebx");
  auto args = simple_instr.getArgs();
  ASSERT_EQ(args.size(), 2);
  invoke_accept(args[0], create_test_visitor("*(<DWORD> _eax*0x8+0x666)"));
  invoke_accept(args[1], create_test_visitor("<DWORD> _ebx"));
}

TEST(DecoderTest, TestXMM) {
  InstructionTemplate simple_instr("mov XMMWORD PTR [eax*0x8+0x666],ebx");
  auto args = simple_instr.getArgs();
  ASSERT_EQ(args.size(), 2);
  invoke_accept(args[0], create_test_visitor("*(<DWORD> _eax*0x8+0x666)"));
  invoke_accept(args[1], create_test_visitor("<DWORD> _ebx"));
}

TEST(DecoderTest, TestLEA) {
  InstructionTemplate simple_instr("jne    4009b0");
  auto args = simple_instr.getArgs();
  ASSERT_EQ(args.size(), 1);
  invoke_accept(args[0], create_test_visitor("4009b0"));
}

struct DummySymbol : public ExecutableFile::symbol_type  {
  DummySymbol()
      : Symbol(nullptr, std::make_shared<befa::Section>(nullptr)) {}

  string getName() const override {
    return "number_of_the_beast";
  }
};

TEST(DecoderTest, TestFunctionFeed) {
  auto dummy_function = std::make_shared<DummySymbol>();
  InstructionTemplate simple_instr("call    0x666");
  std::map<
      bfd_vma, std::shared_ptr<symbol_table::VisitableBase>
  > sym_table {
      std::make_pair(0x666, std::make_shared<symbol_table::Function>(
          dummy_function
      ))
  };
  auto args = simple_instr.getArgs(sym_table);
  ASSERT_EQ(args.size(), 1);
  invoke_accept(args[0], create_test_visitor("@number_of_the_beast"));
}
}
