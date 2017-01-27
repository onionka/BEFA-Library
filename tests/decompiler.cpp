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
struct TestVisitor : public Generalizer<
    llvm::InstructionTraits,
    // Class that generalize derivation classes
    llvm::Instruction,
    // Derivations ...
    llvm::CallInstruction
> {

  TestVisitor(LambdaT &&check)
      : check(std::forward<LambdaT>(check)) {}

  void generalized_visitor(const llvm::Instruction *ptr) override {
    check(ptr);
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
  // instruction created from signature only (dummy)
  InstructionTemplate simple_instr(signature);

  // to create llvm instruction we need factory
  ExecutableFile::symbol_map_type symbol_table{
      symbol_map
  };

  // subject of instruction stream (via this you can send instructions)
  rxcpp::subjects::subject<ExecutableFile::instruction_type> i_subj;

  // create mapper (contains observable - as_observable())
  llvm::InstructionMapper mapper(symbol_table);

  llvm::CallFactory fact(mapper);

  // hook instruction mapper into instruction stream
  i_subj.get_observable().subscribe(
      rxcpp::make_observer_dynamic<ExecutableFile::instruction_type>(mapper)
  );

  bool called = false;
  bool received = false;
  // hook to stream of translated instructions
  mapper.as_observable().subscribe([&](
      const std::shared_ptr<llvm::VisitableBase> &i
  ) {
    received = true;
    invoke_accept(i, create_TestVisitor([&](const auto *instr) {
      ASSERT_EQ(
          instr->getAssembly()[0].getDecoded(),
          simple_instr.getDecoded()
      );
      ASSERT_FALSE(called);
      called = true;
    }));
  });

  ASSERT_FALSE(received);
  ASSERT_FALSE(called);
  // send instruction
  i_subj.get_subscriber().on_next(simple_instr);
  ASSERT_TRUE(received && "not received");
  ASSERT_TRUE(called && "not visited");
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
