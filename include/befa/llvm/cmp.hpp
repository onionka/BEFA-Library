//
// Created by miro on 12/7/16.
//

#ifndef BEFA_CMP_HPP
#define BEFA_CMP_HPP

#include "instruction.hpp"

namespace llvm {

struct CmpInstruction
    : public BinaryOperator {
  enum types_e {
        // greater than     greater or equal
        GT,                 GE,
        // lesser than      lesser or equal
        LT,                 LE,
        // unsigned GT      unsigned GE
        UGT,                UGE,
        // unsigned LT      unsigned LE
        ULT,                ULE,
        // equal            not equal
        EQ,                 NE,
  };

  // FIXME: remove this
  static const std::map<
      std::string,               types_e
  >   comparition_jumps;

  void accept(
      VisitorBase&               visitor
  )   const                      override {
    visitor.visit(this);
  }

  /**
   *
   * @param result where to safe the output of comparition
   * @param lhs left hand side of operation
   * @param op compare operation
   * @param rhs right hand side of operation
   * @param assembly
   */
  CmpInstruction(
      const sym_t::ptr::shared&  result,
      const sym_t::ptr::shared&  lhs,
      types_e                    op,
      const sym_t::ptr::shared&  rhs,
      a_ir_t::c_info  ::ref      assembly
  ) : Serializable(        fetch_name(
      result,         lhs,       op,     rhs
  )), BinaryOperator(           {assembly},
                                 result,
                                 lhs, "cmp", rhs) {}

 private:
  static inline
  std::string              fetch_name(
      const sym_t::ptr::shared&  result,
      const sym_t::ptr::shared&  lhs,
      types_e                    op,
      const sym_t::ptr::shared&  rhs
  );
};

struct CompareFactory
    : public LLVMFactory {
  using symbol_type =            symbol_table::VisitableBase;
  using symbol_ptr  =            std::shared_ptr<symbol_type>;

  void                           operator()(
      a_ir_t::c_info::ref        instruction,
      sym_table_t::ptr::shared   symboL_table,
      ir_t::rx::shared_subs      subscriber
  )   const                      override;
};
}  // namespace llvm

#endif //BEFA_CMP_HPP
