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
  auto pieces = rxcpp::sources::create<std::string>([](
      rxcpp::subscriber<std::string> sub
  ) {
    sub.on_next("mov");
    sub.on_next("eax");
    sub.on_next("ebx");
    sub.on_completed();
  });
  SimpleInstruction simple_instr(pieces);
  simple_instr.getArgs();
}


symbol_table::SymbolVisitorL create_test_visitor(std::string cmp) {
  return symbol_table::SymbolVisitorL(
      [&cmp](const symbol_table::Symbol *ptr) {
        EXPECT_EQ(cmp, ptr->getName());
      }
  );
}

auto create_check_fn(std::string name) {
  return rxcpp::make_observer<std::shared_ptr<symbol_table::VisitableBase>>([&name] (
      const std::shared_ptr<symbol_table::VisitableBase> &ptr
  ) { invoke_accept(ptr, create_test_visitor(name)); });
}

TEST(DecoderTest, WithoutPreparseInstruction) {
  InstructionTemplate simple_instr("mov eax,ebx");
  auto args = simple_instr.getArgs();
  args.count().subscribe([](size_t size) { ASSERT_EQ(2, size); });
  args.element_at(0).subscribe(create_check_fn("((DWORD)_eax)"));
  args.element_at(1).subscribe(create_check_fn("((DWORD)_ebx)"));
}

TEST(DecoderTest, TestDereference) {
  InstructionTemplate simple_instr("mov DWORD PTR [eax*0x8+0x666],ebx");
  auto args = simple_instr.getArgs();
  args.count().subscribe([](size_t size) { ASSERT_EQ(2, size); });
  args.element_at(0).subscribe(create_check_fn("((DWORD)*(((DWORD)_eax)*0x8+0x666))"));
  args.element_at(1).subscribe(create_check_fn("((DWORD)_ebx)"));
}

TEST(DecoderTest, TestXMM) {
  InstructionTemplate simple_instr("mov XMMWORD PTR [eax*0x8+0x666],ebx");
  auto args = simple_instr.getArgs();
  args.count().subscribe([](size_t size) { ASSERT_EQ(2, size); });
  args.element_at(0).subscribe(create_check_fn("((XMMWORD)*(((DWORD)_eax)*0x8+0x666))"));
  args.element_at(1).subscribe(create_check_fn("((DWORD)_ebx)"));
}

TEST(DecoderTest, TestLEA) {
  InstructionTemplate simple_instr("jne    4009b0");
  auto args = simple_instr.getArgs();
  args.count().subscribe([](size_t size) { ASSERT_EQ(1, size); });
  args.element_at(0).subscribe(create_check_fn("4009b0"));
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
  args.count().subscribe([] (size_t size) { ASSERT_EQ(1, size ); });
  args.element_at(0).subscribe(create_check_fn("@number_of_the_beast"));
}
}
