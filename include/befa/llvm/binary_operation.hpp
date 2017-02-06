//
// Created by miro on 2/5/17.
//

#ifndef BEFA_BINARY_OPERATION_HPP
#define BEFA_BINARY_OPERATION_HPP

#include "instruction.hpp"

namespace llvm {

/**
 * @brief Another virtual base class that adds binary operation behaviour to the instruction
 */
struct BinaryOperation
    : virtual public Assignment,
      virtual public Operation,
      virtual public Instruction {
  using bin_op_t = types::traits::container<std::string>;
  using inst_vect_t = Instruction::a_vec_t;
  using Instruction::sym_t;
  using Instruction::a_ir_t;

  BinaryOperation(
      a_ir_t::vector::value assembly,
      sym_t::ptr::shared    target,
      sym_t::ptr::shared    lhs,
      bin_op_t::type        bin_op,
      sym_t::ptr::shared    rhs
  );

  void accept(
      VisitorBase &base
  ) const override {
    base.visit(this);
  }

  // ~~~~~ Implemented overrides (non-final)
  sym_t::ptr::shared     getAssignee()     const override;
  sym_t::ptr::shared     getAssigner()     const override;
  sym_t::vector::shared  getDefinitions()  const override;
  sym_t::vector::shared  getUsedSymbols()  const override;
  a_ir_t::vector::value  getAssembly()     const override;
  operator_t::type       getOperator()     const override;
  std::string             toString()       const override;

 private:
  a_ir_t::vector::value assembly;
  sym_t::ptr::shared  target;
  sym_t::ptr::shared lhs;
  bin_op_t::type bin_op;
  sym_t::ptr::shared rhs;

 protected:
  static inline
  sym_t::ptr::shared createResult(
      sym_t::ptr::shared rhs,
      bin_op_t::type bin_op,
      sym_t::ptr::shared lhs
  );

  static inline
  std::string fetch_name(
      sym_t::ptr::shared target,
      sym_t::ptr::shared lhs,
      bin_op_t::type bin_op,
      sym_t::ptr::shared rhs
  );
};
}  // namespace llvm

#endif //BEFA_BINARY_OPERATION_HPP
