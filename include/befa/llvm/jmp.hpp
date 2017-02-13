//
// Created by miro on 1/15/17.
//

#ifndef BEFA_JMP_HPP
#define BEFA_JMP_HPP

#include "instruction.hpp"
#include "cmp.hpp"


namespace llvm {

struct TerminatorInstruction
    : virtual public Instruction
    , virtual public Serializable
    , virtual public VisitableBase {
  using Instruction::sym_t;

  void accept(VisitorBase &visitor) const override {
    visitor.visit(this);
  }

  virtual sym_t::ptr::shared getTarget() const = 0;

  std::string toString() const override {
    return "Terminator pica";
  }
};

/**
 * Unconditional branch instruction
 */
class JumpInstruction
    :          public TerminatorInstruction
    ,  virtual public User
    ,  virtual public VisitableBase {
 public:
  using TerminatorInstruction::toString;
  using Instruction::sym_t;

  JumpInstruction(
      a_ir_t::vector::value assembly,
      sym_t::ptr::shared target
  ) : assembly(assembly)
    , target(target) {}

  a_ir_t::vector::value getAssembly() const override {
      return assembly;
  }

  void accept(VisitorBase &visitor) const override {
    visitor.visit(this);
  }

  sym_t::ptr::shared getTarget() const override {
    return target;
  }

  std::string toString() const override {
    return "not implemented";
  }

 private:
  a_ir_t::vector::value assembly;
  sym_t::ptr::shared target;
};

/**
 * Conditional branch instruction
 */
class BranchInstruction
    : virtual public VisitableBase,
      virtual public JumpInstruction {
 public:
  using TerminatorInstruction::toString;
  using Instruction::sym_t;

  BranchInstruction(
      a_ir_t::vector::value assembly,
      sym_t::ptr::shared cmp_result,
      sym_t::ptr::shared target
  ) : JumpInstruction(assembly, target)
    , cmp_result(cmp_result) {}

  sym_t::vector::shared getUsedSymbols() const override {
    return {cmp_result, getTarget()};
  }

  sym_t::ptr::shared getCondition() const {
    return cmp_result;
  }

  void accept(VisitorBase &visitor) const override {
    visitor.visit(this);
  }

  std::string toString() const override;

 private:
  a_ir_t::vector::value assembly;
  sym_t::ptr::shared cmp_result;
};


struct JumpFactory
    : public LLVMFactory {
  using symbol_type =            symbol_table::VisitableBase;
  using symbol_ptr  =            std::shared_ptr<symbol_type>;

  void                           operator()(
      a_ir_t::c_info::ref        instruction,
      sym_table_t::ptr::shared   symbol_table,
      ir_t::rx::shared_subs      subscriber
  )   const                      override;
};

}  // namespace llvm

#endif //BEFA_JMP_HPP
