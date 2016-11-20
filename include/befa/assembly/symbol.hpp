//
// Created by miro on 11/10/16.
//

#ifndef BEFA_SYMBOL_HPP
#define BEFA_SYMBOL_HPP

#include "../utils/algorithms.hpp"


namespace befa {

template<typename SectionT>
struct Symbol {
  /**
   * Copy bfd information (so we bind lifetime to this object)
   * @param origin
   */
  Symbol(const asymbol *origin, std::shared_ptr<SectionT> &parent) : origin(origin), parent(parent) {}

  // ~~~~~~~~~~~~~~ Conversions ~~~~~~~~~~~~~~
  Symbol(Symbol<SectionT> &&rhs)
      : origin(std::move(rhs.origin)),
        callees(std::move(rhs.callees)),
        parent(std::move(rhs.parent)) {}

  Symbol &operator=(Symbol<SectionT> &&rhs) {
    origin = std::move(rhs.origin);
    callees = std::move(rhs.callees);
    parent = std::move(rhs.parent);
    return *this;
  }

  Symbol(const Symbol<SectionT> &rhs)
      : origin(rhs.origin),
        callees(rhs.callees),
        parent(rhs.parent) {}

  Symbol &operator=(const Symbol<SectionT> &rhs) {
    origin = rhs.origin;
    callees = rhs.callees;
    parent = rhs.parent;
    return *this;
  }
  // ~~~~~~~~~~~~~~ Conversions ~~~~~~~~~~~~~~

  /**
   * Every symbol have a name
   */
  std::string getName() const { return getOrigin()->name; }

  const asymbol *getOrigin() const { return origin; }

  const std::vector<std::weak_ptr<Symbol<SectionT>>> &getCallees() const { return callees; }

  const std::weak_ptr<SectionT> &getParent() const { return parent; }

  bfd_vma getAddress() const { return bfd_asymbol_value(origin); }

  bfd_vma getDistance(const std::weak_ptr<Symbol> &rhs) const {
    return ptr_lock(rhs)->getAddress() - getAddress();
  }

  bfd_vma getDistance(bfd *fd) const {
    return ptr_lock(getParent())->getSize(fd) - (getAddress() - ptr_lock(getParent())->getAddress(fd));
  }

  flagword getFlags() const { return getOrigin()->flags; }
 private:
  /**
   * BFD asymbol -> this is just an adapter
   *
   * lifetime of this is bound to lifetime of its bfd counterpart (aka what it adapts)
   */
  const asymbol *origin;

  /**
   * Every symbol called from this
   */
  std::vector<std::weak_ptr<Symbol<SectionT>>> callees;

  /**
   * Section to which this symbol belongs to
   */
  std::weak_ptr<SectionT> parent;
};
}  // namespace befa

#endif //BEFA_SYMBOL_HPP
