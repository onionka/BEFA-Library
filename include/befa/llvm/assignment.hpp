//
// Created by miro on 2/5/17.
//

#ifndef BEFA_ASSIGNMENT_HPP
#define BEFA_ASSIGNMENT_HPP

#include "instruction.hpp"


namespace llvm {
/**
 * @brief Represents assignment (ie. mov eax, ebx or result of smkind of operation)
 */
struct Assignment
    : virtual public             Definer
    , virtual public             User
    , virtual public             Serializable {
  using sym_t = Definer::sym_t;

  Assignment() {}

  virtual
  sym_t::ptr::shared          getAssignee()    const = 0;
  virtual
  sym_t::ptr::shared          getAssigner()    const = 0;

  sym_t::vector::shared       getDefinitions() const override {
    return                   {getAssignee()};
  }
  sym_t::vector::shared       getUsedSymbols() const override {
    return                   {getAssigner()};
  }
  std::string                  toString()      const override;

  void                           accept(
      VisitorBase&               base
  )   const                      override {
    base.visit(this);
  }
};

}  // namespace llvm
#endif //BEFA_ASSIGNMENT_HPP
