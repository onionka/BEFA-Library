//
// Created by miro on 11/10/16.
//

#include <algorithm>
#include <dis-asm.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "../../include/befa.hpp"

struct BasicBlockDecoder;

struct SymbolDataLoader {
  typedef ExecutableFile::basic_block_type basic_block_type;
  typedef std::weak_ptr<basic_block_type> basic_block_ptr;

  SymbolDataLoader(
      const std::weak_ptr<ExecutableFile::symbol_type> &ptr
  ) : ptr(ptr_lock(ptr)) {}

  void fetch(RxSubject<basic_block_ptr> &subj, bfd *_fd) {
    // TODO: Firstly, prefetch basic blocks
    printf("%s: %lx\n", ptr->getName().c_str(), ptr->getAddress());
    for (const auto &alias: ptr->getAliases())
      printf("%s: %lx\n", alias.getName().c_str(), alias.getAddress());

    // TODO: Than decode basic blocks
    subj.update(std::make_shared<basic_block_type>(0, ptr));
  }

 private:
  std::shared_ptr<ExecutableFile::symbol_type> ptr;
};

struct BasicBlockDecoder {
  typedef ExecutableFile::instruction_type instruction_type;
  typedef std::weak_ptr<instruction_type> instruction_ptr;

  BasicBlockDecoder(const std::weak_ptr<ExecutableFile::basic_block_type> &ptr)
      : ptr(ptr_lock(ptr)) {}

  void fetch(
      RxSubject<instruction_type> &subj,
      disassemble_info d_info,
      bfd *_fd,
      std::weak_ptr<disassembler_impl::ffile> f,
      bfd_vma sym_size
  ) {
    // TODO: Decode instructions
    { // file related work
      auto f_lock = ptr_lock(f);
      auto sym_lock = ptr_lock(ptr->getParent());
      auto sec_lock = ptr_lock(sym_lock->getParent());

      f_lock->reset();

      disassembler_ftype _dis_asm = disassembler(_fd);
      assert(_dis_asm && "should not be null");

      bfd_vma sym_address = sym_lock->getAddress();
      int i_size = _dis_asm(sym_address, &d_info);
      int offset = 0;
      int max_offset = (int) sym_size;

      // clear fake file, load instruction, ...
      for (uint64_t i_address = sym_address + offset;
           (i_size > 0) && (offset < max_offset);
           f_lock->reset(), i_size = _dis_asm(
               i_address = sym_address + offset,
               &d_info
           )
          ) {
        // create instruction, and pass it into subject
        subj.update(instruction_type(
            array_view<uint8_t>(d_info.buffer + offset, i_size),
            ptr, f_lock->buffer, i_address
        ));
        offset += i_size;
      }
    }
  }

 private:
  std::shared_ptr<ExecutableFile::basic_block_type> ptr;
};

int ffprintf(struct disassembler_impl::ffile *f, const char *format, ...);

disassemble_info create_disassemble_info(bfd *_fd, disassembler_impl::ffile *f);

void ExecutableFile::runDisassembler() {
  auto d_info = create_disassemble_info(_fd, fake_file.get());

  auto sym_table = getSymbolTable();
  for_each(sym_table, [&](auto &sym_ite) {
    auto sym = *sym_ite;
    auto sym_lock = ptr_lock(sym);

    // we doesn't care about non-function symbols
    if (!(sym_lock->hasFlags(BSF_FUNCTION)))
      return;

    auto section_lock = ptr_lock(sym_lock->getParent());
    RxSubject<SymbolDataLoader::basic_block_ptr> bb_subj;

    auto closest_ite = std::find_if(
        sym_ite, sym_table.cend(),
        [&sym](const std::weak_ptr<ExecutableFile::symbol_type> &val) {
          return (ptr_lock(val)->getFlags() & BSF_FUNCTION)
              && (ptr_lock(sym)->getAddress() < ptr_lock(val)->getAddress());
        }
    );

    // find out size of this symbol
    bfd_vma sym_size = closest_ite == sym_table.end()
                       ? sym_lock->getDistance(_fd)
                       : sym_lock->getDistance(*closest_ite);

    d_info.buffer_vma = section_lock->getAddress(_fd);
    d_info.buffer_length = section_lock->getSize(_fd);

    // create shared buffer for section (user of this will get weak_ptr, so lifetime is the same as this file)
    auto memory = new uint8_t[d_info.buffer_length];
    assert(memory && "not enough memory");
    shared_buffer.emplace_back(memory);
    d_info.buffer = memory;

    // loads content into shared data
    bfd_get_section_contents(_fd, (asection *) section_lock->getOrigin(), d_info.buffer, 0, d_info.buffer_length);

    bb_subj.subscribe([&](const SymbolDataLoader::basic_block_ptr &bb_ptr) {
      // decode instructions
      BasicBlockDecoder instruction_decoder(bb_ptr);
      instruction_decoder.fetch(
          assembly_subject, d_info, _fd, fake_file, sym_size
      );
    });
    SymbolDataLoader bb_decoder(sym);
    // decode basic blocks
    bb_decoder.fetch(bb_subj, _fd);
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
    ...) {
  /* Reserve two times as much as the length of the fmt_str */
  int printed, alloc = (int) strlen(format) * 2;
  va_list ap;
  while (1) {
    f->buffer.resize((size_t) alloc + f->pos);
    va_start(ap, format);
    printed = vsnprintf((char *) f->buffer.data() + f->pos, (size_t) alloc, format, ap);
    va_end(ap);
    if (printed < 0 || printed >= alloc)
      alloc += abs(printed - alloc + 1);
    else
      break;
  }
  f->pos += printed;
  return printed;
}

disassemble_info create_disassemble_info(bfd *_fd, disassembler_impl::ffile *f) {
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