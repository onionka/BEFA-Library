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
      int id,
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

  const std::weak_ptr<SymbolT> &getParent() const {
    return parent;
  }

  const std::vector<std::weak_ptr<BasicBlock<SymbolT>>> &getSuccessors() const {
    return successors;
  }

  int getId() const {
    return id;
  }

 private:
  std::weak_ptr<SymbolT> parent;

  std::vector<std::weak_ptr<BasicBlock<SymbolT>>> successors;

  int id;
};
}  // namespace befa

#endif //BEFA_BASIC_BLOCK_HPP
