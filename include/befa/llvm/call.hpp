//
// Created by miro on 11/30/16.
//

#ifndef BEFA_LLVM_UNARY_HPP
#define BEFA_LLVM_UNARY_HPP

#include "instruction.hpp"
#include "unary_instruction.hpp"
#include "../../befa.hpp"

namespace llvm {
class CallInstruction
    : public               UnaryInstruction {

  /**
   * @brief is visitor for all types of symbols
   */
  using generalizer_t =    symbol_table::SymbolVisitorL;

 public:

  static const operator_t::type an_operator;

  /**
   * @brief Represents call instruction from LLVM IR
   * @param assembly is real representation
   * @param target
   * @param result is variable in which should be a result
   */
  CallInstruction(
      const a_vec_t&       assembly,
      sym_t::ptr::shared   result,
      sym_t::ptr::shared   call_target
  );

  std::string            toString() const override;

  void                     accept(
      VisitorBase&         visitor
  )   const                override {
    visitor.visit(this);
  }
};

struct CallFactory
    : public                    LLVMFactory {
  void                          operator()(
      a_ir_t::c_info::ref       i,
      sym_table_t::ptr::shared  symbol_table,
      ir_t::rx::shared_subs     subscriber
  )   const                     override;
};

}  // namespace llvm

#endif //BEFA_LLVM_UNARY_HPP
