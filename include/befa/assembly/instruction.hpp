//
// Created by miro on 11/10/16.
//

#ifndef BEFA_INSTRUCTION_HPP
#define BEFA_INSTRUCTION_HPP

#include <memory>
#include <pcrecpp.h>
#include <rxcpp/rx-observable.hpp>

#include "../utils/algorithms.hpp"
#include "../utils/byte_array_view.hpp"
#include "instruction_parser.hpp"

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
inline auto split(
    const std::string &str,
    const pcrecpp::RE &parse_regex
);
}  // namespace details

template<
    typename                  BasicBlockT
>
struct Instruction
    : public                  instruction_parser {
  using self     =            Instruction                     <BasicBlockT>;
  using base     =            instruction_parser;
  using bb_t     =            types::traits::container        <BasicBlockT>;
  using bytes_t  =            array_view                      <uint8_t>;

  // Dummy instruction
  Instruction()  =            default;

  Instruction(
      bytes_t                 bytes    ,
      typename
      bb_t::ptr::weak         parent   ,
      std::string             decoded  ,
      bfd_vma                 address
  ) : bytes                  (bytes   ),
      parent                 (parent  ),
      decoded                (decoded ),
      address                (address ) {}

  // ~~~~~~~~~~~~~~ Conversions ~~~~~~~~~~~~~~
  Instruction(
      self&&              rhs
  ) : bytes    (std::move(rhs.bytes  )),
      parent   (std::move(rhs.parent )),
      decoded  (std::move(rhs.decoded)),
      address  (std::move(rhs.address)) {}

  Instruction&                operator=(
      self&&              rhs
  ) {
    bytes     = std::move(rhs.bytes  );
    parent    = std::move(rhs.parent );
    decoded   = std::move(rhs.decoded);
    address   = std::move(rhs.address);
    return                   *this    ;
  }

  Instruction(
      const self&         rhs
  ) : bytes              (rhs.bytes  ),
      parent             (rhs.parent ),
      decoded            (rhs.decoded),
      address            (rhs.address) {}

  Instruction&               operator=(
      const self&         rhs
  ) {
    bytes               = rhs.bytes  ;
    parent              = rhs.parent ;
    decoded             = rhs.decoded;
    address             = rhs.address;
    return                   *this   ;
  }
  // ~~~~~~~~~~~~~~ Conversions ~~~~~~~~~~~~~~

  // ~~~~~~~~~~~~~~ Getters ~~~~~~~~~~~~~~
  const bytes_t&     getBytes()   const { return bytes; }

  const std::string& getDecoded() const { return decoded; }

  bfd_vma            getAddress() const { return address; }

  piece_t::rx::obs   parse()      const   override {
    return details::split(getDecoded(), parse_regex);
  }

  typename
  bb_t::ptr::shared  getParent()  const { return ptr_lock(parent); }
  // ~~~~~~~~~~~~~~ Getters ~~~~~~~~~~~~~~

  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~
  bool               operator==  (const self& rhs) const noexcept {
    return rhs.getAddress() == getAddress();
  }

  bool               operator!=  (const self& rhs) const noexcept {
    return          !(*this == rhs);
  }
  // ~~~~~~~~~~~~~~ Operators ~~~~~~~~~~~~~~

 private:
  // ~~~~~~~~~~~~~~ Fields ~~~~~~~~~~~~~~

  /**
   * Raw data
   */
  bytes_t                     bytes;

  /**
   * Basic block to which instruction belongs
   */
  typename bb_t::ptr::weak    parent;

  /**
   * Decoded data in human readable representation (intel-syntax assembly language)
   */
  std::string                 decoded;

  /**
   * Address relative to file
   */
  bfd_vma                     address;
  // ~~~~~~~~~~~~~~ Fields ~~~~~~~~~~~~~~
};

namespace details {
/**
 * Splits into groups
 *
 * @param str to be split by regular expression
 * @param parse_regex is pcree
 * @return observable of parsed pieces
 */
auto split(
    const std::string& str,
    const pcrecpp::RE& parse_regex
) {
  return rxcpp::sources::create<std::string>(
      [&] (rxcpp::subscriber<std::string> s) {
        string temp;
        pcrecpp::StringPiece input(str);
        while (parse_regex.FindAndConsume(&input, &temp))
          s.on_next(temp);
        s.on_completed();
      }
  );
}
}  // namespace details
}  // namespace befa

#endif //BEFA_INSTRUCTION_HPP
