//
// Created by miro on 12/7/16.
//

#ifndef BEFA_CMP_HPP
#define BEFA_CMP_HPP

#include "instruction.hpp"

namespace llvm {

struct CmpInstruction
    : public Instruction,
      virtual public llvm::VisitableBase,
      public Serializable {
  enum types_e {
    // greater than
    GT, GE,
    // lesser than
    LT, LE,
    // unsigned greater than
    UGT, UGE,
    // unsigned lesser than
    ULT, ULE,
    // (not) equal
    EQ, NE,
  };

  CmpInstruction(
      const std::shared_ptr<symbol_table::VisitableBase> &result,
      const std::shared_ptr<symbol_table::VisitableBase> &lhs,
      types_e op,
      const std::shared_ptr<symbol_table::VisitableBase> &rhs,
      const instruction_type &asembly
  ) : Instruction({asembly}), Serializable(fetchSignature(
      result, lhs, op, rhs
  )), result(result), lhs(lhs), op(op), rhs(rhs) {}


  void accept(VisitorBase &visitor) override {
    visitor.visit(this);
  }

 private:
  std::string fetchSignature(
      const std::shared_ptr<symbol_table::VisitableBase> &result,
      const std::shared_ptr<symbol_table::VisitableBase> &lhs,
      types_e op,
      const std::shared_ptr<symbol_table::VisitableBase> &rhs
  ) const {
    static std::map<types_e, std::string> type_to_str {
        {GT, "gt"},
        {GE, "ge"},
        {LT, "lt"},
        {LE, "le"},
        {UGT, "ugt"},
        {UGE, "uge"},
        {ULT, "ult"},
        {ULE, "ule"},
        {EQ, "eq"},
        {NE, "ne"}
    };

    std::string signature;
    symbol_table::SymbolVisitorL arg_visitor(
        [&signature] (const symbol_table::Symbol *sym) {
          signature += sym->getName();
        }
    );
    lhs->accept(arg_visitor);
    signature += " " + type_to_str[op] + " ";
    rhs->accept(arg_visitor);
    return signature;
  }


  const std::shared_ptr<symbol_table::VisitableBase> &result;
  const std::shared_ptr<symbol_table::VisitableBase> &lhs;
  types_e op;
  const std::shared_ptr<symbol_table::VisitableBase> &rhs;
};

struct ICmpInstruction final
    : public CmpInstruction,
      virtual public llvm::VisitableBase {
  const static std::map<std::string, types_e> comparition_jumps;

  void accept(VisitorBase &visitor) override {
    visitor.visit(this);
  }

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
      typename lhsIntegralT, typename rhsIntegralT
  >
  ICmpInstruction(
      const std::shared_ptr<symbol_table::Variable> &result,
      const std::shared_ptr<symbol_table::SizedSymbol<lhsIntegralT>> &lhs,
      types_e op,
      const std::shared_ptr<symbol_table::SizedSymbol<rhsIntegralT>> &rhs,
      const instruction_type &parent
  ) : CmpInstruction(
      result, lhs, op, rhs, parent
  ) {}
};

struct FCmpInstruction final
    : public CmpInstruction,
      virtual public llvm::VisitableBase {
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
      typename lhsFloatT, typename rhsFloatT
  >
  FCmpInstruction(
      const std::shared_ptr<symbol_table::Variable> &result,
      const std::shared_ptr<symbol_table::SizedSymbol<lhsFloatT>> &lhs,
      types_e op,
      const std::shared_ptr<symbol_table::SizedSymbol<rhsFloatT>> &rhs,
      const instruction_type &parent
  ) : CmpInstruction(
      result, lhs, op, rhs, parent
  ) {}

  void accept(VisitorBase &visitor) override {
    visitor.visit(this);
  }
};

}

#endif //BEFA_CMP_HPP
