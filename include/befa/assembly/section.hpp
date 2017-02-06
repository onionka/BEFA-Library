//
// Created by miro on 11/10/16.
//

#ifndef BEFA_SECTION_HPP
#define BEFA_SECTION_HPP

#include <dis-asm.h>
namespace befa {

struct Section {
  Section(const asection *origin) : origin(origin) {}

  std::string getName() { return origin->name; }

  const asection *getOrigin() const { return origin; }

  size_t getSize(ATTRIBUTE_UNUSED bfd *fd) const { return bfd_section_size(fd, getOrigin()); }

  bfd_vma getAddress(ATTRIBUTE_UNUSED bfd *fd=nullptr) const { return bfd_get_section_vma(fd, getOrigin()); }


  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~
  bool operator==(const Section &rhs) const noexcept {
    return rhs.getOrigin() == getOrigin();
  }

  bool operator!=(const Section &rhs) const noexcept {
    return !(rhs == *this);
  }
  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~
 private:
  const asection *origin;
};
}  // namespace befa

#endif //BEFA_SECTION_HPP
