//
// Created by miro on 11/10/16.
//

#ifndef BEFA_INSTRUCTION_HPP
#define BEFA_INSTRUCTION_HPP

#include <memory>
#include <pcrecpp.h>

#include "../utils/algorithms.hpp"
#include "../utils/byte_array_view.hpp"

namespace befa {
static const ::pcrecpp::RE parse_regex = std::string(
    ",?(?|"
        // 'DWORD PTR [rip+0x9421596]' ... '# 0x00000(2156113)'
        // result will be ('2156113', )
        "\\w+\\sPTR\\s\\[rip\\+[^\\],]+\\](?=.*#\\s0x0*([a-f0-9]+))" "|"
        // '(DWORD PTR [...])'
        // result will be ('DWORD PTR [...]', )
        "(\\w+\\sPTR\\s\\[[^\\],]+\\])" "|"
        // '0x000000(542312)'
        // result will be ('542312', )
        "(?<!#\\s)0x0*([a-f0-9]+)" "|"
        // the rest ... (ie. registers)
        "(?<!#\\s)(\\w+)"
    "),?"
);

template<typename BasicBlockT>
struct Instruction {
  // Dummy instruction
  Instruction() = default;

  Instruction(
      ::array_view<uint8_t> bytes,
      std::shared_ptr<BasicBlockT> parent,
      std::string decoded,
      bfd_vma address
  ) : bytes(bytes),
      decoded(decoded),
      parent(parent),
      address(address) {}

  // ~~~~~~~~~~~~~~ Conversions ~~~~~~~~~~~~~~
  Instruction(Instruction<BasicBlockT> &&rhs)
      : bytes(std::move(rhs.bytes)),
        decoded(std::move(rhs.decoded)),
        parent(std::move(rhs.parent)),
        address(std::move(rhs.address)) {}

  Instruction &operator=(Instruction<BasicBlockT> &&rhs) {
    bytes = std::move(rhs.bytes);
    decoded = std::move(rhs.decoded);
    parent = std::move(rhs.parent);
    address = std::move(rhs.address);
    return *this;
  }

  Instruction(const Instruction<BasicBlockT> &rhs)
      : bytes(rhs.bytes),
        decoded(rhs.decoded),
        parent(rhs.parent),
        address(rhs.address) {}

  Instruction &operator=(const Instruction<BasicBlockT> &rhs) {
    bytes = rhs.bytes;
    decoded = rhs.decoded;
    parent = rhs.parent;
    address = rhs.address;
    return *this;
  }
  // ~~~~~~~~~~~~~~ Conversions ~~~~~~~~~~~~~~

  // ~~~~~~~~~~~~~~ Getters ~~~~~~~~~~~~~~
  const ::array_view<uint8_t> &getBytes() const { return bytes; }

  const std::string &getDecoded() const { return decoded; }

  const std::shared_ptr<BasicBlockT> &getParent() const { return parent; }

  const bfd_vma &getAddress() const { return address; }

  ::std::vector<::std::string> parse() const {
    ::std::vector<::std::string> result;
    ::std::string temp;
    pcrecpp::StringPiece input(getDecoded());
    while (parse_regex.FindAndConsume(&input, &temp)) {
      result.push_back(temp);
    }
    return result;
  }

  // ~~~~~~~~~~~~~~ Getters ~~~~~~~~~~~~~~
 private:
  // ~~~~~~~~~~~~~~ Fields ~~~~~~~~~~~~~~

  /**
   * Raw data
   */
  ::array_view<uint8_t> bytes;

  /**
   * Decoded data in human readable representation (intel-syntax assembly language)
   */
  std::string decoded;

  /**
   * Basic block to which instruction belongs
   */
  std::shared_ptr<BasicBlockT> parent;

  /**
   * Address relative to file
   */
  bfd_vma address;
  // ~~~~~~~~~~~~~~ Fields ~~~~~~~~~~~~~~
};
}  // namespace befa

#endif //BEFA_INSTRUCTION_HPP
