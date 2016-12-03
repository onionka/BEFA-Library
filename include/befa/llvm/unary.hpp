//
// Created by miro on 11/30/16.
//

#ifndef BEFA_LLVM_UNARY_HPP
#define BEFA_LLVM_UNARY_HPP

#include "befa/utils/factory.hpp"
#include "instruction.hpp"


namespace llvm {
struct CallInstruction final
    : public VisitableImpl<CallInstruction>,
      // traits
      Instruction {
  using function_type = ExecutableFile::symbol_type;

  CallInstruction() {}

  std::shared_ptr<function_type> getTarget() const {
    return ptr_lock(target);
  }

  string getSignature() const override {
    return "call @" + getTarget()->getName() + "()";
  }

  bfd_vma getAddress() const override {
    return address;
  }

  std::shared_ptr<BasicBlock> getBasicBlock() override {
    return nullptr;
  }

 private:
  std::weak_ptr<function_type> target;
  std::weak_ptr<BasicBlock> bb;
  bfd_vma address;
};


template<>
struct Factory<CallInstruction> final {
  std::shared_ptr<CallInstruction> create(
      const std::vector<std::string> &input
  ) {
    return std::make_shared<CallInstruction>();
  }
};

template<>
struct InstructionMapper<CallInstruction> final
    : public InstructionMapperBase<CallInstruction> {
  InstructionMapper(
      InstructionFactory &factory,
      LLVMBuilder &builder
  ) : InstructionMapperBase(factory, builder) { }

  void operator() (const ExecutableFile::instruction_type &i) {
    auto parsed_i = i.parse();
    if (parsed_i[0] == "call")
      builder.send_instruction(factory.create(parsed_i));
  }
};

}

#endif //BEFA_LLVM_UNARY_HPP
