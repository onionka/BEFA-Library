//
// Created by miro on 12/7/16.
//

#ifndef BEFA_CMP_HPP
#define BEFA_CMP_HPP

#include "instruction.hpp"

namespace llvm {

struct CmpInstruction : public Instruction {
  enum types_e {
    // greater than
        GT, GE,

    // lesser than
        LT, LE,

    // (not) equal
        EQ, NE,
  };

  const std::vector<std::string> type_to_str{
      ">", ">=", "<", "<=", "==", "!=", "jo"
  };

  const std::vector<std::string> type_to_jmp{

      "ja", "jae", "jb", "jbe", "jc", "jcxz", "je", "jecxz", "jg", "jge",
      "jl", "jle", "jna", "jnae", "jnb", "jnbe", "jnc", "jne", "jng",
      "jnge", "jnl", "jnle", "jno", "jnp", "jns", "jnz", "jo", "jp", "jpe",
      "jpo", "js", "jz", "jmp"
  };

  CmpInstruction(
      const std::shared_ptr<symbol_table::VisitableBase> &lhs,
      types_e op,
      const std::shared_ptr<symbol_table::VisitableBase> &rhs,
      const instruction_type &parent
  ) : Instruction({parent}), lhs(lhs), op(op), rhs(rhs) {}

  std::string getSignature() const {
    std::string signature;
    ArgVisitor arg_visitor(signature);
    lhs->accept(arg_visitor);
    signature += " " + type_to_str[op] + " ";
    rhs->accept(arg_visitor);
    return signature;
  }

 private:
  struct ArgVisitor : public symbol_table::VisitorBase {
    ArgVisitor(std::string &name_out) : name_out(name_out) {}

    ASM_VISIT_ALL(arg) {
      name_out += arg->getName();
    }

   private:
    std::string &name_out;
  };

  const std::shared_ptr<symbol_table::VisitableBase> &lhs;
  types_e op;
  const std::shared_ptr<symbol_table::VisitableBase> &rhs;
};

struct ICmpInstruction final
    : public CmpInstruction, public llvm::VisitableImpl<ICmpInstruction> {
  /**
   *
   * @param lhs is left hand size of int compare operation
   * @param op is compare operation
   * @param rhs is right hand size of int compare operation
   * @param parent is reference to the asm instruction from
   *        which this has been derived
   * @tparam lhsIntegralT param should be deduced
   * @tparam lhsSymbolBase param should be deduced
   * @tparam rhsIntegralT param should be deduced
   * @tparam rhsSymbolBase param should be deduced
   */
  template<
      typename lhsIntegralT, typename lhsSymbolBase,
      typename rhsIntegralT, typename rhsSymbolBase
  >
  ICmpInstruction(
      const std::shared_ptr<symbol_table::SizedSymbol<
          lhsIntegralT, lhsSymbolBase>> &lhs,
      types_e op,
      const std::shared_ptr<symbol_table::SizedSymbol<
          rhsIntegralT, rhsSymbolBase>> &rhs,
      const instruction_type &parent
  ) : CmpInstruction(
      static_cast<
          typename std::enable_if<
              symbol_table::types::is_integral<lhsIntegralT>::value,
              lhsSymbolBase &>::type
          >(lhs), op,
      static_cast<
          typename std::enable_if<
              symbol_table::types::is_integral<lhsIntegralT>::value,
              rhsSymbolBase &>::type
          >(rhs), parent) {}
};

struct FCmpInstruction final
    : public CmpInstruction, public llvm::VisitableImpl<FCmpInstruction> {
  /**
   *
   * @param lhs is left hand size of float compare operation
   * @param op is compare operation
   * @param rhs is right hand size of float compare operation
   * @param parent is reference to the asm instruction from
   *        which this has been derived
   * @tparam lhsFloatT param should be deduced
   * @tparam lhsSymbolBase param should be deduced
   * @tparam rhsFloatT param should be deduced
   * @tparam rhsSymbolBase param should be deduced
   */
  template<
      typename lhsFloatT, typename lhsSymbolBase,
      typename rhsFloatT, typename rhsSymbolBase
  >
  FCmpInstruction(
      const std::shared_ptr<symbol_table::SizedSymbol<
          lhsFloatT, lhsSymbolBase>> &lhs,
      types_e op,
      const std::shared_ptr<symbol_table::SizedSymbol<
          rhsFloatT, rhsSymbolBase>> &rhs,
      const instruction_type &parent
  ) : CmpInstruction(
      static_cast<
          typename std::enable_if<
              symbol_table::types::is_sse<lhsFloatT>::value,
              lhsSymbolBase &>::type
          >(lhs), op,
      static_cast<
          typename std::enable_if<
              symbol_table::types::is_sse<rhsFloatT>::value,
              rhsSymbolBase &>::type
          >(rhs), parent) {}
};

}

#endif //BEFA_CMP_HPP
