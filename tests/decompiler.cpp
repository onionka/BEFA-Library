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

  void generalized_visitor(
      const llvm::Instruction *ptr
  ) override { check(ptr); }

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
    std::vector<std::string> signatures,
    ExecutableFile::symbol_map_type symbol_map,
    std::string compare
) {
  // instruction created from signature only (dummy)
  auto instruction_stream = rxcpp::sources::iterate(signatures)
      .map([](std::string signature) {
        return InstructionTemplate(signature);
      });

  // to create llvm instruction we need factory
  auto symbol_table =
      std::make_shared<ExecutableFile::symbol_map_type>(symbol_map);

  // subject of instruction stream (via this you can send instructions)
  rxcpp::subjects::subject<ExecutableFile::instruction_type> i_subj;

  // create mapper (contains observable - as_observable())
  std::shared_ptr<llvm::InstructionMapper> mapper
      = std::make_shared<llvm::InstructionMapper>(symbol_table);

  // append Call factory
  mapper->register_factory(
      std::make_shared<llvm::CallFactory>(mapper)
  );

  // hook instruction mapper into instruction stream
  mapper->subscribe_on(i_subj.get_observable());

  // send instruction
  instruction_stream.subscribe(
      [&](const ExecutableFile::instruction_type &simple_i) {
        bool tested = false;
        bool received = false;
        // hook to stream of translated instructions
        mapper
            // get observable from mapper
            ->observable()

                // subscribe for newly generated instructions
            .subscribe([&](
                const std::shared_ptr<llvm::VisitableBase> &i
            ) {
              received = true;
              // retrieve type of instruction
              invoke_accept(i, create_TestVisitor([&](
                  const llvm::Instruction *instr
              ) -> void {
                ASSERT_EQ(
                    instr->getAssembly()[0].getDecoded(),
                    simple_i.getDecoded()
                );
                ASSERT_FALSE(tested);
                tested = true;
              }));
            });

        ASSERT_FALSE(received);
        ASSERT_FALSE(tested);

        i_subj.get_subscriber().on_next(simple_i);

        ASSERT_TRUE(received && "not received");
        ASSERT_TRUE(tested && "not visited");
      }
  );
}

TEST(DecompilerTest, CallTest) {
  test_single_instruction(
      {"call    400800"},
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

TEST(DecompilerTest, CmpTest) {

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
