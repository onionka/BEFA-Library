//
// Created by miro on 11/10/16.
//

#include <algorithm>
#include <dis-asm.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sstream>
#include <map>
#include <iostream>
#include <set>

#include "../../include/befa/utils/assert.hpp"
#include "../../include/befa.hpp"

struct BasicBlockDecoder;

/**
 *
 * @param instr
 * @return address that instruction is jumping to (else -1 which is FFFFFF...)
 */
uint64_t match_jump(std::string instr) {
  static const ::pcrecpp::RE regex(
      "(?:"
          "ja|jae|jb|jbe|jc|jcxz|je|jecxz|jg|jge|jl|jle|jna|jnae|jnb|jnbe|"
          "jnc|jne|jng|jnge|jnl|jnle|jno|jnp|jns|jnz|jo|jp|jpe|jpo|js|jz|jmp"
          ")\\s+(?|"
          "\\w+ PTR \\[rip[^\\]]+\\]\\s+# 0x0*([0-9a-f]+)" "|"
          "0x0*([0-9a-f]+)" "|"
          "(.*)"
          ")"
  );
  static const ::std::vector<::std::string> jumps{
      "ja", "jae", "jb", "jbe", "jc", "jcxz", "je", "jecxz", "jg", "jge",
      "jl", "jle", "jna", "jnae", "jnb", "jnbe", "jnc", "jne", "jng",
      "jnge", "jnl", "jnle", "jno", "jnp", "jns", "jnz", "jo", "jp", "jpe",
      "jpo", "js", "jz", "jmp"
  };

  uint64_t addr = 0;
  std::string addr_str;
  pcrecpp::StringPiece input(instr);
  if (regex.FindAndConsume(&input, &addr_str)) {
    std::stringstream ss;
    ss << std::hex << addr_str;
    if (ss >> addr)
      return addr;
    else
      return (uint64_t) -2;
  }
  return (uint64_t) -1;
}

struct SymbolDataLoader {
  using sym_t = ExecutableFile::sym_t;
  using bb_t = ExecutableFile::bb_t;
  using inst_t = ExecutableFile::inst_t;

  using ffile_t = type_traits::container<disassembler_impl::ffile>;

  SymbolDataLoader(
      sym_t::ptr::weak ptr
                  ) : ptr(ptr) {}

  void fetch(
      inst_t::rx::subj &instr_subj,
      bb_t::vector::shared &basic_block_buffer,
      disassemble_info d_info,
      bfd *_fd,
      ffile_t::ptr::weak f,
      bfd_vma sym_size
            ) {
    { // file related work
      auto f_lock = ptr_lock(f);
      auto sym_lock = ptr_lock(ptr);
      auto sec_lock = ptr_lock(sym_lock->getParent());

      f_lock->reset();

      disassembler_ftype _dis_asm = disassembler(_fd);
      assert_ex(
          _dis_asm,
          "failed to open file descriptor for disassembling"
      );

      bfd_vma sym_address = sym_lock->getAddress();
      int i_size = _dis_asm(sym_address, &d_info);
      int offset = 0;
      int max_offset = (int) sym_size;

      std::vector<std::tuple<array_view<uint8_t>, std::string, uint64_t>>
          instructions;
      std::set<uint64_t> basic_block_addresses{sym_address};

      // clear fake file, load instruction, ...
      for (uint64_t i_address = sym_address + offset;
           (i_size > 0) && (offset < max_offset);
           f_lock->reset(), i_size = _dis_asm(
               i_address = sym_address + offset,
               &d_info
           )
          ) {
        uint64_t address;
        if ((address = match_jump(f_lock->buffer)) != (uint64_t) -1) {
          basic_block_addresses.emplace(address);
          basic_block_addresses.emplace(i_address + i_size);
        }
        // create instruction, and pass it into subj
        instructions.emplace_back(std::make_tuple(
            array_view<uint8_t>(d_info.buffer + offset, i_size),
            f_lock->buffer,
            i_address
        ));
        offset += i_size;
      }
      // erase -1 (which is 0xFFFFFF)
      auto bba_begin = basic_block_addresses.begin();

      for (auto &instr : instructions) {
        // if instruction has a border address, basic block is created
        if (bba_begin != basic_block_addresses.end() &&
            std::get<2>(instr) == *bba_begin) {
          basic_block_buffer.emplace_back(
              std::make_shared<bb_t::info::type>(*bba_begin, ptr)
          );
          ++bba_begin;
        }
        assert_ex(
            !basic_block_buffer.empty(),
            "basic_block_buffer cannot be empty"
        );
        instr_subj.get_subscriber()
                  .on_next(inst_t::info::type(
                      std::get<0>(instr), basic_block_buffer.back(),
                      std::get<1>(instr), std::get<2>(instr)
                  ));
      }
    }
  }

 private:
  sym_t::ptr::weak ptr;
};

int ffprintf(struct disassembler_impl::ffile *f, const char *format, ...);

disassemble_info create_disassemble_info(bfd *_fd, disassembler_impl::ffile *f);

void ExecutableFile::runDisassembler() {
  auto d_info = create_disassemble_info(_fd, fake_file.get());

  auto sym_table = getSymbolTable();
  for_each(sym_table, [&](auto &sym_ite) {
    sym_t::ptr::weak sym = *sym_ite;
    sym_t::ptr::shared sym_lock = ptr_lock(sym);

    // we doesn't care about non-function symbols
    if (!(sym_lock->hasFlags(BSF_FUNCTION)))
      return;

    sec_t::ptr::shared section_lock = ptr_lock(sym_lock->getParent());

    sym_t::vector::weak::const_iterator closest_ite = std::find_if(
        sym_ite, sym_table.cend(),
        [&sym](const sym_t::ptr::weak &val) {
          return (ptr_lock(val)->getFlags() & BSF_FUNCTION)
              && (ptr_lock(sym)->getAddress() < ptr_lock(val)->getAddress());
        }
    );

    // find out size of this symbol
    bfd_vma sym_size = closest_ite == sym_table.end()
                       ? sym_lock->getDistance(_fd)
                       : sym_lock->getDistance(*closest_ite);

    d_info.buffer_vma = section_lock->getAddress(_fd);
    d_info.buffer_length = (unsigned int) section_lock->getSize(_fd);

    // create shared buffer for section (user of this will get weak_ptr, so lifetime is the same as this file)
    auto memory = new uint8_t[d_info.buffer_length];
    assert_ex(memory, "not enough memory");
    shared_buffer.emplace_back(memory);
    d_info.buffer = memory;

    // loads content into shared data
    bfd_get_section_contents(
        _fd, (asection *) section_lock->getOrigin(),
        d_info.buffer, 0, d_info.buffer_length
    );

    // decode basic blocks
    SymbolDataLoader(sym).fetch(
        assembly_subject, basic_block_buffer, d_info, _fd, fake_file, sym_size
    );
  });
}

disassembler_impl::ffile::ffile() : pos(0) {}

void disassembler_impl::ffile::reset() {
  pos = 0;
  buffer.clear();
}

int ffprintf(
    struct disassembler_impl::ffile *f,
    const char *format,
    ...
            ) {
  int printed, alloc = (int) strlen(format);
  va_list ap;
  while (1) {
    f->buffer
     .resize((size_t) alloc + f->pos);
    va_start(ap, format);
    printed = vsnprintf(
        (char *) f->buffer
                  .data() + f->pos,
        (size_t) alloc,
        format,
        ap);
    va_end(ap);
    if (printed < 0 || printed >= alloc)
      alloc += abs(printed - alloc + 1);
    else
      break;
  }
  f->pos += printed;
  return printed;
}

disassemble_info create_disassemble_info(
    bfd *_fd, disassembler_impl::ffile *f
                                        ) {
  disassemble_info ret;
  init_disassemble_info(&ret, f, (fprintf_ftype) ffprintf);

  // some bfd settings
  ret.flavour = bfd_get_flavour(_fd);
  ret.arch = bfd_get_arch(_fd);
  ret.mach = bfd_get_mach(_fd);
  ret.octets_per_byte = bfd_octets_per_byte(_fd);
  ret.skip_zeroes = 8;
  ret.skip_zeroes_at_end = 3;
  ret.application_data = _fd;

  // Endianess
  if (bfd_big_endian(_fd))
    ret.display_endian = BFD_ENDIAN_BIG;
  else if (bfd_little_endian(_fd))
    ret.display_endian = BFD_ENDIAN_LITTLE;
  else
    ret.endian = BFD_ENDIAN_UNKNOWN;
  ret.disassembler_options = (char *) "-M intel,intel-mnemonic";
  return ret;
}