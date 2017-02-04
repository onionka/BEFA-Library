//
// Created by miro on 11/10/16.
//

#ifndef BEFA_HPP
#define BEFA_HPP

#include <memory>
#include <vector>
#include <bfd.h>
#undef GCC_VERSION
#include <rxcpp/rx.hpp>

#include "befa/utils/types.hpp"
#include "befa/assembly/instruction.hpp"
#include "befa/assembly/basic_block.hpp"
#include "befa/assembly/symbol.hpp"
#include "befa/assembly/section.hpp"

namespace llvm {
/**
 * defined in @see befa/llvm/instruction.hpp
 */
struct InstructionMapper;
}

/**
 * BFD old-c algorthms adaptor
 */
class disassembler_impl {
 public:
  struct ffile {
    ffile();
    void reset();
    std::string buffer;
    size_t pos;
  };

  // ~~~~~ Copy & Move semantics
  disassembler_impl(const disassembler_impl &) = delete;
  disassembler_impl &operator=(const disassembler_impl &) = delete;
  disassembler_impl(disassembler_impl &&rhs);

  disassembler_impl &operator=(disassembler_impl &&rhs);
  // ~~~~~ Copy & Move semantics

 private:
  std::vector<asection *> sections;
  std::vector<asymbol *> symbol_table;
  std::vector<asymbol> synthetic_symbol_table;
  std::shared_ptr<asymbol> _syn_sym_table;

 protected:

  bfd *_fd;

  std::shared_ptr<ffile> fake_file = std::make_shared<ffile>();

  std::vector<std::unique_ptr<uint8_t[]>> shared_buffer;

  ~disassembler_impl() { if (_fd) bfd_close(_fd); }
 public:
  disassembler_impl(bfd *file_descriptor);

  std::vector<asection *> &fetchSections();

  std::vector<asymbol *> &fetchSymbolTable();
};

namespace type_tags = types::tags;
namespace type_traits = types::traits;

class ExecutableFile
    : private disassembler_impl {
 public:
  // ~~~~~ Traits
  using sec_t = type_traits::container<
      befa::Section
  >;

  using sym_t = type_traits::container<
      befa::Symbol<sec_t::info::type>
  >;

  using bb_t = type_traits::container<
      befa::BasicBlock<sym_t::info::type>
  >;

  using inst_t = type_traits::container<
      befa::Instruction<bb_t::info::type>
  >;

  using mapper_t = type_traits::container<
      llvm::InstructionMapper
  >;

  using var_t = type_traits::container<
      symbol_table::VisitableBase
  >;

  using map_t = type_traits::container<
      typename var_t::map<std::string>::shared
  >;
  // ~~~~~ Traits

  /**
   * Opens a file to be disassebled (creates instance of this)
   * @param path to the file
   * @param target architecture of binary file
   * @return created disassembly object of a file
   * @raises std::runtime_error
   */
  static ExecutableFile open(
      std::string path,
      std::string target = ""
  );

  ExecutableFile(ExecutableFile &&rhs);

  ExecutableFile &operator=(ExecutableFile &&rhs);

  /**
   *
   * @return array of targets
   */
  static std::vector<std::string> getTargets();

  /**
   *
   * @param fd
   */
  ExecutableFile(bfd *fd);

  /**
   * If file descriptor is valid
   * @return true if fd is not null
   */
  bool isValid() const { return is_valid; }

  /**
   *
   * @return assembly instruction stream (observable)
   */
  inst_t::rx::obs disassembly() const {
    return assembly_subject.get_observable();
  }

  /**
   * Executes disassembler
   * @return observable to the instruction stream
   */
  void runDisassembler();

  /**
   * Feed this into getArgs, so it will know where (ie. call) want's to jump
   *
   * @return table of name of symbol and its visitable
   */
  var_t::map<bfd_vma>::shared generate_table();

  /**
   *
   * @return
   */
  void runDecompiler();

  /**
   * Generates symbol table
   *  (use it max once)
   * @return
   */
  sym_t::vector::weak getSymbolTable();

  /**
   * Generates sections
   *  (use it max once)
   * @return
   */
  sec_t::vector::weak getSections();

 private:
  /**
   * Subject of instructions (see reactive programming)
   */
  inst_t::rx::subj assembly_subject;

  /**
   * Subject of llvm instructions (see reactive programming)
   */
  mapper_t::ptr::shared llvm_mapper;

  /**
   * Here are stored shared pointers at sections
   */
  sec_t::vector::shared section_buffer;

  /**
   * Here are stored shared pointers at sections
   */
  sym_t::vector::shared symbol_buffer;

  /**
   * Here are stored shared pointers at sections
   */
  bb_t::vector::shared basic_block_buffer;

  /**
   * If this instance has valid file descriptor
   */
  bool is_valid;

  /**
   * bfd_init should not be called more then once
   */
  static bool bfd_was_init;

  /**
   * if getSection has been sorted
   */
  bool sections_sorted = false;

  /**
   * if getSymbol has been sorted
   */
  bool symbols_sorted = false;
};

#endif  // BEFA_HPP
