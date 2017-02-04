//
// Created by miro on 1/15/17.
//

#ifndef BEFA_JMP_HPP
#define BEFA_JMP_HPP

#include "instruction.hpp"


namespace llvm {

struct TerminatorInstruction
    : public Instruction,
      public Serializable,
      virtual public VisitableBase {
  TerminatorInstruction(
      const std::vector<instruction_type> &assembly
  ) : Instruction(assembly), Serializable("") {}
};

/**
 * Unconditional branch instruction
 */
class JumpInstruction
    : public TerminatorInstruction,
      virtual public VisitableBase {
 public:
  JumpInstruction(
      const std::vector<instruction_ptr> &assembly
  ) : TerminatorInstruction(assembly) {}

  void accept(VisitorBase &visitor) const override {
    visitor.visit(this);
  }
};

/**
 * Conditional branch instruction
 */
class BranchInstruction
    : public TerminatorInstruction,
      virtual public VisitableBase {
 public:
  BranchInstruction(
      const std::vector<instruction_ptr> &assembly
  ) : TerminatorInstruction(assembly) {}

  void accept(VisitorBase &visitor) const override {
    visitor.visit(this);
  }
};
}  // namespace llvm

#endif //BEFA_JMP_HPP
