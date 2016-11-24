//
// Created by miro on 11/10/16.
//

#ifndef BEFA_HPP
#define BEFA_HPP

#include <memory>
#include <vector>
#include <bfd.h>
#include "befa/utils/observer.hpp"
#include "befa/llvm/instruction.hpp"
#include "befa/assembly/instruction.hpp"
#include "befa/assembly/basic_block.hpp"
#include "befa/assembly/symbol.hpp"
#include "befa/assembly/section.hpp"

/**
 * BFD old-c algorthms adaptor
 */
class disassembler_impl {
 public:
  struct ffile {
    ffile();
    void reset();
    ::std::string buffer;
    size_t pos;
  };

  disassembler_impl(const disassembler_impl &) = delete;
  disassembler_impl &operator=(const disassembler_impl &) = delete;
  disassembler_impl(disassembler_impl &&rhs)
      : sections(std::move(sections)),
        symbol_table(std::move(symbol_table)),
        synthetic_symbol_table(std::move(synthetic_symbol_table)),
        _syn_sym_table(std::move(_syn_sym_table)),
        _fd(rhs._fd),
        fake_file(std::move(rhs.fake_file)),
        shared_buffer(std::move(rhs.shared_buffer)) {
    rhs._fd = nullptr;
  }

  disassembler_impl &operator=(disassembler_impl &&rhs) {
    sections = std::move(sections);
    symbol_table = std::move(symbol_table);
    synthetic_symbol_table = std::move(synthetic_symbol_table);
    _syn_sym_table = std::move(_syn_sym_table);
    _fd = rhs._fd;
    fake_file = std::move(rhs.fake_file);
    shared_buffer = std::move(rhs.shared_buffer);
    rhs._fd = nullptr;
    return *this;
  }

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

class ExecutableFile
    : private disassembler_impl {
 public:
  typedef befa::Section section_type;
  typedef befa::Symbol<section_type> symbol_type;
  typedef befa::BasicBlock<symbol_type> basic_block_type;
  typedef befa::Instruction<basic_block_type> instruction_type;

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

  ExecutableFile(ExecutableFile &&rhs)
      : disassembler_impl(std::move(rhs)),
        assembly_subject(std::move(rhs.assembly_subject)),
        llvm_instructions(std::move(rhs.llvm_instructions)),
        basic_block_subj(std::move(rhs.basic_block_subj)),
        section_buffer(std::move(rhs.section_buffer)),
        symbol_buffer(std::move(rhs.symbol_buffer)),
        is_valid(std::move(rhs.is_valid)),
        sections_sorted(std::move(rhs.sections_sorted)),
        symbols_sorted(std::move(rhs.symbols_sorted)) {}

  ExecutableFile &operator=(ExecutableFile &&rhs) {
    disassembler_impl::operator=(std::move(rhs));
    assembly_subject = std::move(rhs.assembly_subject);
    llvm_instructions = std::move(rhs.llvm_instructions);
    basic_block_subj = std::move(rhs.basic_block_subj);
    section_buffer = std::move(rhs.section_buffer);
    symbol_buffer = std::move(rhs.symbol_buffer);
    is_valid = std::move(rhs.is_valid);
    sections_sorted = std::move(rhs.sections_sorted);
    symbols_sorted = std::move(rhs.symbols_sorted);
    return *this;
  }

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
   * @return Observable of instructions
   */
  RxObservable<instruction_type> &disassembly() { return assembly_subject.asObservable(); }

  /**
   *
   * @return Observable of instructions
   */
  RxObservable<std::vector<instruction_type>> &basic_block() { return basic_block_subject.asObservable(); }

  /**
   *
   * @return Observable of instructions
   */
  RxObservable<llvm::Instruction> &llvm() { return llvm_instructions.asObservable(); }

  /**
   * Executes disassembler
   * @return observable to the instruction stream
   */
  void runDisassembler();

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
  std::vector<std::weak_ptr<symbol_type>> getSymbolTable();

  /**
   * Generates sections
   *  (use it max once)
   * @return
   */
  std::vector<std::weak_ptr<section_type>> getSections();

 private:
  /**
   * Subject of instructions (see reactive programming)
   */
  RxSubject<instruction_type> assembly_subject;

  /**
   * Subject of llvm instructions (see reactive programming)
   */
  RxSubject<llvm::Instruction> llvm_instructions;

  /**
   * Subject of basic blocks (see reactive programming)
   */
  RxSubject<std::weak_ptr<basic_block_type>> basic_block_subj;

  /**
   * Subject of the same basic block instructions
   */
  RxSubject<std::vector<instruction_type>> basic_block_subject;

  /**
   * Here are stored shared pointers at sections
   */
  std::vector<std::shared_ptr<section_type>> section_buffer;

  /**
   * Here are stored shared pointers at sections
   */
  std::vector<std::shared_ptr<symbol_type>> symbol_buffer;

  /**
   * Here are stored shared pointers at sections
   */
  std::vector<std::shared_ptr<basic_block_type>> basic_block_buffer;

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
