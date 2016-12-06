//
// Created by miro on 11/30/16.
//

#ifndef BEFA_LLVM_UNARY_HPP
#define BEFA_LLVM_UNARY_HPP

#include "instruction.hpp"
#include "mapper.hpp"
#include "../../befa.hpp"

namespace llvm {
struct CallInstruction final
    : public VisitableImpl<CallInstruction>,
        // traits
      Instruction {
  using symbol_type = ExecutableFile::symbol_type;
  using instruction_type = ExecutableFile::instruction_type;
  using basic_block_type = ExecutableFile::basic_block_type;

  CallInstruction(
      std::weak_ptr<symbol_type> target,
      const instruction_type &assembly
  ) : target(target), assembly(assembly) {}

  std::shared_ptr<symbol_type> getTarget() const {
    return ptr_lock(target);
  }

  const instruction_type &getAssembly() const /*override*/ {
    return assembly;
  }

  std::shared_ptr<basic_block_type> getParent() const /*override*/ {
    return getAssembly().getParent();
  }

  string getSignature() const /*override*/ {
    return "call @" + getTarget()->getName() + "()";
  }

  bfd_vma getAddress() const /*override*/ {
    return getTarget()->getAddress();
  }

 private:
  std::weak_ptr<symbol_type> target;
  instruction_type assembly;
};

}  // namespace llvm

#endif //BEFA_LLVM_UNARY_HPP
