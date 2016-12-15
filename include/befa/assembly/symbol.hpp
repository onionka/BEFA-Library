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
  Symbol(const asymbol *origin, const std::shared_ptr<SectionT> &parent)
      : origin(
      origin), parent(parent) {}

  // ~~~~~~~~~~~~~~ Conversions ~~~~~~~~~~~~~~
  Symbol(Symbol<SectionT> &&rhs)
      : origin(std::move(rhs.origin)),
        parent(std::move(rhs.parent)) {}

  Symbol &operator=(Symbol<SectionT> &&rhs) {
    origin = std::move(rhs.origin);
    parent = std::move(rhs.parent);
    return *this;
  }

  Symbol(const Symbol<SectionT> &rhs)
      : origin(rhs.origin),
        parent(rhs.parent) {}

  Symbol &operator=(const Symbol<SectionT> &rhs) {
    origin = rhs.origin;
    parent = rhs.parent;
    return *this;
  }
  // ~~~~~~~~~~~~~~ Conversions ~~~~~~~~~~~~~~

  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~
  bool operator==(const Symbol<SectionT> &rhs) const noexcept {
    return rhs.getOrigin() == getOrigin();
  }

  bool operator!=(const Symbol<SectionT> &rhs) const noexcept {
    return !(rhs == *this);
  }
  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~


  // ~~~~~~~~~~~~~~ Getters ~~~~~~~~~~~~~~
  virtual /**
   * Every symbol have a name
   */
  std::string getName() const { return getOrigin()->name; }

  const asymbol *getOrigin() const { return origin; }

  const std::weak_ptr<SectionT> &getParent() const { return parent; }

  virtual bfd_vma getAddress() const { return bfd_asymbol_value(origin); }

  bfd_vma getDistance(const std::weak_ptr<Symbol> &rhs) const {
    return ptr_lock(rhs)->getAddress() - getAddress();
  }

  bfd_vma getDistance(bfd *fd) const {
    return ptr_lock(getParent())->getSize(fd)
        - (getAddress() - ptr_lock(getParent())->getAddress(fd));
  }

  flagword getFlags() const { return getOrigin()->flags; }

  bool hasFlags(const flagword &flag) const {
    return getFlags() & flag ||
        (std::find_if(
            aliases.cbegin(), aliases.cend(),
            [&flag](const asymbol *alias)
            { return alias->flags & flag; }
        ) != aliases.cend());
  }

  // ~~~~~~~~~~~~~~ Aliases ~~~~~~~~~~~~~~
  std::vector<Symbol> getAliases() const {
    return ::map(
        aliases, [&](const auto &alias)
        { return Symbol<SectionT>(alias, ptr_lock(parent)); }
    );
  }
  // ~~~~~~~~~~~~~~ Getters ~~~~~~~~~~~~~~

  template<typename LambdaT>
  void iter_aliases(LambdaT &&pred) const {
    std::for_each(
        aliases.cbegin(), aliases.cend(),
        [&pred](const auto &alias)
        { pred(Symbol<SectionT>(alias)); }
    );
  }

  void addAlias(asymbol *alias) {
    aliases.push_back(alias);
  }
  // ~~~~~~~~~~~~~~ Aliases ~~~~~~~~~~~~~~

  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~
  bool operator==(const std::string &name) const {
    return getName() == name ||
        std::find_if(
            aliases.cbegin(), aliases.cend(),
            [&name](const asymbol *alias) {
              return name == alias->name;
            }
        ) != aliases.cend();
  }

  bool operator!=(const std::string &name) const {
    return !(*this == name);
  }
  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~
 private:
  /**
   * BFD asymbol -> this is just an adapter
   *
   * lifetime of this is bound to lifetime of its bfd counterpart (aka what it adapts)
   */
  const asymbol *origin;

  /**
   * Section to which this symbol belongs to
   */
  std::weak_ptr<SectionT> parent;

  /**
   * Alias holder
   */
  std::vector<asymbol *> aliases;
};
}  // namespace befa

#endif //BEFA_SYMBOL_HPP
