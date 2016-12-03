//
// Created by miro on 11/10/16.
//

#ifndef BEFA_INSTRUCTION_HPP
#define BEFA_INSTRUCTION_HPP

#include <memory>
#include <pcrecpp.h>

#include "../utils/algorithms.hpp"
#include "../utils/byte_array_view.hpp"
#include "instruction_decoder.hpp"

namespace befa {
static const ::pcrecpp::RE parse_regex = std::string(
    "(?|" "(\\w*\\.\\w+)" "|"
        "# 0x0*[0-9a-fA-F]+" "|"
        "(\\w+ PTR (?:\\w+:)?\\[[^\\]]+\\])" "|"
        "\\[([^]]+)\\]" "|"
        "0x0*?([0-9a-fA-F]+)" "|"
        "(\\w+\\w+\\w+)" "|"
        "(\\w+)"
        ")"
);

namespace details {
inline std::vector<std::string> match(
    const std::string &str,
    const pcrecpp::RE &parse_regex
);
}  // namespace details

template<typename BasicBlockT>
struct Instruction
    : public instruction_decoder {
  using basic_block = BasicBlockT;
  using basic_block_weak = std::weak_ptr<basic_block>;
  using basic_block_ptr = std::shared_ptr<basic_block>;
  using byte_array = ::array_view<uint8_t>;

  // Dummy instruction
  Instruction() = default;

  Instruction(
      byte_array bytes,
      basic_block_ptr parent,
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
  const byte_array &getBytes() const { return bytes; }

  const std::string &getDecoded() const { return decoded; }

  std::shared_ptr<BasicBlockT> getParent() const { return ptr_lock(parent); }

  const bfd_vma &getAddress() const { return address; }

  instruction_pieces parse() const override {
    return details::match(getDecoded(), parse_regex);
  }
  // ~~~~~~~~~~~~~~ Getters ~~~~~~~~~~~~~~

  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~
  bool operator==(const Instruction<BasicBlockT> &rhs) const noexcept {
    return rhs.getAddress() == getAddress();
  }

  bool operator!=(const Instruction<BasicBlockT> &rhs) const noexcept {
    return !(rhs == *this);
  }
  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~

 private:
  // ~~~~~~~~~~~~~~ Fields ~~~~~~~~~~~~~~

  /**
   * Raw data
   */
  byte_array bytes;

  /**
   * Decoded data in human readable representation (intel-syntax assembly language)
   */
  std::string decoded;

  /**
   * Basic block to which instruction belongs
   */
  basic_block_weak parent;

  /**
   * Address relative to file
   */
  bfd_vma address;
  // ~~~~~~~~~~~~~~ Fields ~~~~~~~~~~~~~~
};

namespace details {
std::vector<std::string> match(
    const std::string &str,
    const pcrecpp::RE &parse_regex
) {
  std::vector<std::string> result;
  string temp;
  pcrecpp::StringPiece input(str);
  while (parse_regex.FindAndConsume(&input, &temp))
    result.push_back(temp);
  return result;
}
}  // namespace details
}  // namespace befa

#endif //BEFA_INSTRUCTION_HPP
