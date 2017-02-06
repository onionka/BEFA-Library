//
// Created by miro on 2/5/17.
//

#ifndef BEFA_UNARY_INSTRUCTION_HPP
#define BEFA_UNARY_INSTRUCTION_HPP

#include "instruction.hpp"
#include "assignment.hpp"

namespace llvm {
/**
 * @brief Represents instructions that works only with one
 *        operand
 */
struct UnaryInstruction
    : virtual public Assignment,
      virtual public Operation,
      virtual public Instruction {
  using Instruction::sym_t;

  /**
   * @brief ie. dereference, call, ...
   * @param assembly
   * @param target
   * @param _operator
   * @param operand
   */
  UnaryInstruction(
      a_ir_t::vector::value assembly,
      sym_t::ptr::shared target,
      operator_t::type _operator,
      sym_t::ptr::shared operand
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

  // ~~~~~ Optional requirement
  virtual
  sym_t::ptr::shared     getResult()       const {
    return UnaryInstruction::get_result(
        getOperator(), getUsedSymbols()[0]
    );
  }

  static inline
  std::shared_ptr<
      symbol_table::        Temporary
  >    get_result(
    operator_t::type       _operator,
    sym_t::ptr::shared      operand
  ) {
    // TODO: add type of result ...
    return std::make_shared<
        symbol_table::Temporary
    >(_operator, operand);
  }

 private:
  a_ir_t::vector::value     assembly;
  sym_t::ptr::shared        target;
  operator_t::type         _operator;
  sym_t::ptr::shared        operand;

 private:
  static inline
  std::string          fetchName(
      sym_t::ptr::shared    target,
      operator_t::type     _operator,
      sym_t::ptr::shared    operand
  );

};
}  // namespace llvm

#endif //BEFA_UNARY_INSTRUCTION_HPP
