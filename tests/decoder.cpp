//
// Created by miro on 12/3/16.
//

#include <iostream>
#include <gtest/gtest.h>
#include <memory>

#define INSTRUCTION_TEST

#include <befa/utils/visitor.hpp>
#include <befa/assembly/asm_arg_parser.hpp>
#include <befa/utils/factory.hpp>
#include <befa/assembly/instruction.hpp>

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

TEST(DecoderTest, WithoutPreparseInstruction) {
  NonPreparsedInstruction simple_instr("mov eax,ebx");
  simple_instr.getArgs();
}
}
