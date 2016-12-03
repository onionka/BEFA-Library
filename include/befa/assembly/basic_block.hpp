//
// Created by miro on 11/10/16.
//

#ifndef BEFA_BASIC_BLOCK_HPP
#define BEFA_BASIC_BLOCK_HPP


#include <memory>
#include "../utils/algorithms.hpp"

namespace befa {

template<typename SymbolT>
struct BasicBlock {
  BasicBlock(
      bfd_vma id,
      const std::weak_ptr<SymbolT> &parent
  ) : parent(parent),
      id(id) {}

  // ~~~~~~~~~~~~~~ Conversions ~~~~~~~~~~~~~~
  BasicBlock(BasicBlock<SymbolT> &&rhs)
      : parent(std::move(rhs.parent)),
        successors(rhs.successors),
        id(std::move(rhs.id)) {}

  BasicBlock &operator=(BasicBlock<SymbolT> &&rhs) {
    parent = std::move(rhs.parent);
    successors = rhs.successors;
    id = std::move(rhs.id);
    return *this;
  }

  BasicBlock(const BasicBlock<SymbolT> &rhs)
      : parent(rhs.parent),
        successors(rhs.successors),
        id(rhs.id) {}

  BasicBlock &operator=(const BasicBlock<SymbolT> &rhs) {
    parent = rhs.parent;
    successors = rhs.successors;
    id = rhs.id;
    return *this;
  }
  // ~~~~~~~~~~~~~~ Conversions ~~~~~~~~~~~~~~

  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~
  bool operator==(const BasicBlock<SymbolT> &rhs) const noexcept {
    return rhs.getId() == getId();
  }

  bool operator!=(const BasicBlock<SymbolT> &rhs) const noexcept {
    return !(rhs == *this);
  }
  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~

  // ~~~~~~~~~~~~~~ Getters ~~~~~~~~~~~~~~
  const std::weak_ptr<SymbolT> &getParent() const {
    return parent;
  }

  const std::vector<std::weak_ptr<BasicBlock<SymbolT>>> &getSuccessors() const {
    return successors;
  }

  /** AKA virtual memory address of first instruction */
  bfd_vma getId() const { return id; }
  // ~~~~~~~~~~~~~~ Getters ~~~~~~~~~~~~~~

 private:
  std::weak_ptr<SymbolT> parent;

  std::vector<std::weak_ptr<BasicBlock<SymbolT>>> successors;

  bfd_vma id;
};
}  // namespace befa

#endif //BEFA_BASIC_BLOCK_HPP
