//
// Created by miro on 11/10/16.
//


#include <cassert>
#include <malloc.h>
#include <algorithm>

#include "../../include/befa/utils/algorithms.hpp"
#include "../../include/befa.hpp"

bool ExecutableFile::bfd_was_init = false;

// ~~~~~~~~~~~ ExecutableFile implementation ~~~~~~~~~~~
ExecutableFile ExecutableFile::open(
    std::string path,
    std::string target
) {
  bfd *fd;

  // multi-init prevent
  if (!bfd_was_init) {
    bfd_was_init = true;
    bfd_init();
  }

  // if target not defined, pick the first one
  if (target == "")
    target = getTargets().front();
  else {
    auto targets(getTargets());
    assert(
        std::find(targets.cbegin(), targets.cend(), target) != targets.cend() &&
            "bad target"
    );
  }

  // open file for read
  if ((fd = bfd_openr(path.c_str(), target.c_str())) == NULL)
    throw std::runtime_error(std::string("cannot open '") + path + "'!\nbfd_openr returned NULL");

  // this HAS TO be called, because bfd will get SIGSEGV otherwise
  // bfd - fuck the logic
  if (!bfd_check_format(fd, bfd_object)) {
    bfd_close(fd);
    throw std::runtime_error(
        std::string("cannot open '") + path + "' as bfd_object\nbfd_check_format returned false"
    );
  }

  // creates adapter on file descriptor
  return std::move(ExecutableFile(fd));
}

std::vector<std::weak_ptr<ExecutableFile::symbol_type>> ExecutableFile::getSymbolTable() {
  if (symbol_buffer.empty())
    for_each(fetchSymbolTable(), [&](auto &sym_ite) {
      auto &sym = *sym_ite;
      ExecutableFile::symbol_type *existing_sym = nullptr;
      if (!contains_if(symbol_buffer, [&](auto &shared_s) {
        existing_sym = shared_s.get();
        return shared_s->getAddress() == bfd_asymbol_value(sym);
      })) {
        // if symbol doesn't exists, create new one
        // Firstly find section in section_buffer that symbol is pointing to
        auto sec_ite = std::find_if(
            section_buffer.begin(), section_buffer.end(),
            [&sym](std::shared_ptr<section_type> &sec) { return sec->getOrigin() == sym->section; }
        );
        // if section has been buffered, just use it
        if (sec_ite != section_buffer.end()) {
          symbol_buffer.push_back(std::make_shared<symbol_type>(sym, *sec_ite));
        } else {
          // else create it
          section_buffer.emplace_back(std::make_shared<section_type>(sym->section));
          symbol_buffer.push_back(std::make_shared<symbol_type>(sym, section_buffer.back()));
        }
      } else {
        existing_sym->addAlias(sym);
      }
    });

  // guard to prevent multi-sort
  if (!symbols_sorted) {
    // sort section_buffer in descending
    std::sort(symbol_buffer.begin(), symbol_buffer.end(), [&](auto &lsh, auto &rhs) {
      return bfd_asymbol_value(lsh->getOrigin()) < bfd_asymbol_value(rhs->getOrigin());
    });
    symbols_sorted = true;
  }

  return map(symbol_buffer, [](auto &shared_s) { return std::weak_ptr<symbol_type>(shared_s); });
}

std::vector<std::string> ExecutableFile::getTargets() {
  std::vector<std::string> ret;
  auto c_targets = bfd_target_list();
  for (auto ite = c_targets; *ite; ++ite)
    ret.push_back(*ite);
  free(c_targets);
  return ret;
}


struct BasicBlockSubscriber {
  BasicBlockSubscriber(
      const RxSubject<std::vector<ExecutableFile::instruction_type>> &basic_block_subject
  ) : basic_block_subject(basic_block_subject) {}

  void operator()(const ExecutableFile::instruction_type &instr) {
    auto &locked_bb = instr.getParent();
    if (bb_id == -1)
      bb_id = locked_bb->getId();
    // if parent is different, than the first instruction
    // send to the subject whole basic block vector of instructions
    if (bb_id != locked_bb->getId()) {
      basic_block_subject.update(buffer);
      buffer.clear();
      bb_id = locked_bb->getId();
    } else
      buffer.push_back(instr);
  }

 private:
  RxSubject<std::vector<ExecutableFile::instruction_type>> basic_block_subject;
  std::vector<ExecutableFile::instruction_type> buffer;
  int bb_id = -1;
};


ExecutableFile::ExecutableFile(bfd *fd)
    : disassembler_impl(fd), is_valid(fd != NULL) {
  assembly_subject.subscribe(BasicBlockSubscriber(basic_block_subject));
}

std::vector<std::weak_ptr<ExecutableFile::section_type>> ExecutableFile::getSections() {
  // append missing sections into section buffer
  for_each(fetchSections(), [&](auto &sec_ite) {
    auto sec = *sec_ite;
    if (!contains_if(section_buffer, [&sec](auto &shared_s) { return shared_s->getOrigin() == sec; }))
      // if section is not in section_buffer, create it!
      section_buffer.emplace_back(std::make_shared<section_type>(sec));
  });

  // sort section_buffer in descending
  if (!sections_sorted) {
    std::sort(
        section_buffer.begin(),
        section_buffer.end(), [&](
            const std::shared_ptr<section_type> &lsh,
            const std::shared_ptr<section_type> &rhs) {
          return bfd_section_vma(_fd, lsh->getOrigin()) > bfd_section_vma(_fd, rhs->getOrigin());
        });
    sections_sorted = true;
  }

  return map(section_buffer, [](auto &shared_s) { return std::weak_ptr<section_type>(shared_s); });
}
// ~~~~~~~~~~~ ExecutableFile implementation ~~~~~~~~~~~

// ~~~~~~~~~~~ disassembler_impl implementation ~~~~~~~~~~~
disassembler_impl::disassembler_impl(bfd *file_descriptor)
    : _fd(file_descriptor) {}

std::vector<asymbol *> &disassembler_impl::fetchSymbolTable() {
  // skip fetch if fetched already
  if (!symbol_table.empty()) return symbol_table;

  long storage_needed;
  long dyn_storage_needed;
  long number_of_symbols;
  long number_of_dyn_sym;
  long synthetic_count;
  size_t i = 0;

  // Storage needed to allocate sym table
  storage_needed = bfd_get_symtab_upper_bound(_fd);
  if (storage_needed < 0)
    throw std::runtime_error(bfd_errmsg(bfd_get_error()));

  dyn_storage_needed = bfd_get_dynamic_symtab_upper_bound(_fd);
  if (dyn_storage_needed < 0 && (bfd_get_file_flags (_fd) & DYNAMIC))
    throw std::runtime_error(bfd_errmsg(bfd_get_error()));
  if (dyn_storage_needed < 0)
    dyn_storage_needed = 0;

  if (storage_needed == 0 && dyn_storage_needed == 0)
    return symbol_table;

  // Allocate storage for normal and dynamic symbols
  symbol_table.resize(
      static_cast<size_t>(storage_needed + dyn_storage_needed)
          / sizeof(asymbol *));

  // Canonicalize symbol table
  if (storage_needed != 0) {
    number_of_symbols = bfd_canonicalize_symtab(_fd, symbol_table.data());
    if (number_of_symbols < 0)
      number_of_symbols = 0;
  } else
    number_of_symbols = 0;

  // Canonicalize (dynamic) symbol table
  if (dyn_storage_needed != 0) {
    number_of_dyn_sym = bfd_canonicalize_dynamic_symtab(
        _fd, symbol_table.data() + number_of_symbols);
    if (number_of_dyn_sym < 0)
      number_of_dyn_sym = 0;
  } else
    number_of_dyn_sym = 0;

  // There is a bug, which cause symbol table to have invalid
  // extra pointers (zeros), we have to erase them. THANKS TO BFD
  if (number_of_symbols + number_of_dyn_sym
      < (long) ((storage_needed + dyn_storage_needed) / sizeof(asymbol *)))
    symbol_table.erase(
        symbol_table.begin() + number_of_symbols + number_of_dyn_sym,
        symbol_table.end());

  // File may have synthetic sym table - see BFD doc for more info
  asymbol *temp_syn_table;
  synthetic_count = bfd_get_synthetic_symtab(
      _fd, number_of_symbols, symbol_table.data(), number_of_dyn_sym,
      symbol_table.data() + number_of_symbols, &temp_syn_table);

  // sometimes there are no synthetic symbols ...
  if (synthetic_count < 0)
    synthetic_count = 0;

  if (synthetic_count > 0) {
    synthetic_symbol_table.resize((size_t) synthetic_count);
    symbol_table.resize(static_cast<size_t>(
                            synthetic_count + number_of_symbols
                                + number_of_dyn_sym));
    for (auto _sym =
        symbol_table.begin() + number_of_dyn_sym + number_of_symbols;
         _sym != symbol_table.end();
         ++_sym, ++i) {
      synthetic_symbol_table[i] = temp_syn_table[i];
      *_sym = synthetic_symbol_table.data() + i;
    }
  }

  _syn_sym_table.reset(temp_syn_table, free);

  // Not all symbols which section has are in symbol table
  // Section symbol like .text not always contain .text symbol
  // It is maybe bug in BFD or feature ...
  //    long time later ...:
  //      values of these symbols are fffffffffff so they are totaly useless
  /*
  for (auto sec_ptr = _fd->sections; sec_ptr != NULL; sec_ptr = sec_ptr->next)
    if (sec_ptr->symbol != nullptr)
      symbol_table.push_back(sec_ptr->symbol);
      */

  // Needs to be sorted by address
  //  ::std::sort(symbol_table.begin(), symbol_table.end(),
  //              [](const asymbol *_1, const asymbol *_2) -> bool {
  //                return _1->value < _2->value;
  //              });
  return symbol_table;
}

std::vector<asection *> &disassembler_impl::fetchSections() {
  // if fetched, just return result
  if (!sections.empty()) return sections;

  sections.emplace_back(bfd_com_section_ptr);
  sections.emplace_back(bfd_und_section_ptr);
  sections.emplace_back(bfd_abs_section_ptr);
  sections.emplace_back(bfd_ind_section_ptr);
  for (auto sec_ptr = _fd->sections;
       sec_ptr != NULL;
       sec_ptr = sec_ptr->next)
    sections.emplace_back(sec_ptr);
  return sections;
}
// ~~~~~~~~~~~ disassembler_impl implementation ~~~~~~~~~~~
