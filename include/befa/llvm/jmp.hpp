//
// Created by miro on 1/15/17.
//

#ifndef BEFA_JMP_HPP
#define BEFA_JMP_HPP

#include "instruction.hpp"


namespace llvm {

struct TerminatorInstruction
    : virtual public Serializable,
      virtual public VisitableBase {
  void accept(VisitorBase &visitor) const override {
    visitor.visit(this);
  }

  std::string toString() const override {
    return "Terminator pica";
  }
};

/**
 * Unconditional branch instruction
 */
class JumpInstruction
    :         public Instruction,
              public TerminatorInstruction,
      virtual public VisitableBase {
 public:
  using TerminatorInstruction::toString;

  JumpInstruction(
      a_ir_t::vector::value assembly
  ) : assembly(assembly) {}

  a_ir_t::vector::value getAssembly() const override {
      return assembly;
  }

  void accept(VisitorBase &visitor) const override {
    visitor.visit(this);
  }

  std::string toString() const override {
    return "Terminator pica";
  }

 private:
  a_ir_t::vector::value assembly;
};

/**
 * Conditional branch instruction
 */
class BranchInstruction
    :         public Instruction,
              public TerminatorInstruction,
      virtual public VisitableBase {
 public:
  using TerminatorInstruction::toString;

  BranchInstruction(
      a_ir_t::vector::value assembly
  ) : assembly(assembly) {}

  a_ir_t::vector::value getAssembly() const override {
    return assembly;
  }

  void accept(VisitorBase &visitor) const override {
    visitor.visit(this);
  }

  std::string toString() const override {
    return "Terminator pica";
  }

 private:
  a_ir_t::vector::value assembly;
};
}  // namespace llvm

#endif //BEFA_JMP_HPP
