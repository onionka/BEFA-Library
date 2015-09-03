/**
 * @file file.cpp
 * @author Miroslav Cibulka
 * @brief Here is executable File class implemented
 */

#include "../include/bfp.hpp"
#include <algorithm>
#include <cstdarg>


#define DEFAULT_BUFFER_SIZE 2048
namespace bfp
  {
      ffile File::_FFILE;

      File::File(
          bfd *fd,
          const char *path,
          const char *target)
          :
          _fd{fd},
          _path{path},
          _target{target}
        {
          _buffer.resize(DEFAULT_BUFFER_SIZE);
          setDisassembleInfo();
          retrieve_symbols();
        }

      File::~File()
        {
          bfd_close(_fd);
          free(synthetic_symbol_table);
        }

      File::iterator File::begin()
        {
          asection *_sec = bfd_und_section_ptr;
          iterator _ite(this, 0);
          _ite->_sec = _sec;
          if (_ite->hasContent())
            {
              size_t _size = static_cast<size_t>(_ite->getContentSize());
              if (_buffer.size() < _size)
                {
                  size_t reserve = _buffer.size();
                  while (reserve < _size) reserve += DEFAULT_BUFFER_SIZE;
                  _buffer.resize(reserve);
                }
              bfd_get_section_contents(_fd, _sec, _buffer, 0, _size);
              _ite->_data = _buffer;
            }
          else
            {
              _ite->_data = nullptr;
            }
          _ite->_symbols = get_sym_from_sec(_sec);
          _ite->_dis_asm = getDisassembler();
          _ite->_dis_info = &_dis_asm_info;
          return _ite;
        }

      void File::next(
          Section &_sec,
          File::difference_type &offset)
        {
          if ((offset += 1) == size())
            return;
          asection *_s;
          switch (offset)
            {
              case 1:
                _s = bfd_abs_section_ptr;
              break;
              case 2:
                _s = bfd_com_section_ptr;
              break;
              case 3:
                _s = bfd_ind_section_ptr;
              break;
              default:
                _s = _fd->sections;
              for (difference_type i = 0;
                   i < offset - 4 && i < size() && _s != nullptr;
                   i++)
                _s = _s->next;
              if (_s == nullptr)
                {
                  offset = size();
                  return;
                }
            }
          _sec._sec = _s;
          if (_sec.hasContent())
            {
              size_t _size = _sec.getContentSize();
              if (_buffer.size() < _size)
                {
                  size_t reserve = _buffer.size();
                  while (reserve < _size) reserve += DEFAULT_BUFFER_SIZE;
                  _buffer.resize(reserve);
                }
              bfd_get_section_contents(_fd, _s, _buffer, 0, _size);
              _sec._data = _buffer;
            }
          else
            {
              _sec._data = nullptr;
            }
          _s->vma = bfd_get_section_vma(_fd, _s);
          _sec._symbols = get_sym_from_sec(_s);
          _sec._dis_asm = getDisassembler();
          _sec._dis_info = &_dis_asm_info;
        }

      File::iterator File::end()
        {
          return iterator(this, size());
        }

      File::difference_type File::capacity()
        {
          return size();
        }

      File::difference_type File::size()
        {
          return _fd->section_count + 4;
        }

      File::difference_type File::max_size()
        {
          return size();
        }

      File::value_type File::operator[](int n)
        {
          iterator _ite = begin();
          for (int i = 0;
               i < n;
               i++)
            _ite++;
          return *_ite;
        }

      ::std::string File::get_path() const
        {
          return _path;
        }

      ::std::string File::get_target() const
        {
          return _target;
        }

      void File::setDisassembleInfo()
        {
          init_disassemble_info(&_dis_asm_info, &_FFILE,
                                (fprintf_ftype) ffprintf);

          /*some bfd settings*/
          _dis_asm_info.flavour = bfd_get_flavour (_fd);
          _dis_asm_info.arch = bfd_get_arch(_fd);
          _dis_asm_info.mach = bfd_get_mach(_fd);
          _dis_asm_info.octets_per_byte = bfd_octets_per_byte(_fd);
          _dis_asm_info.skip_zeroes = 8;
          _dis_asm_info.skip_zeroes_at_end = 3;
          _dis_asm_info.application_data = _fd;

          if (bfd_big_endian (_fd))
            _dis_asm_info.display_endian = BFD_ENDIAN_BIG;
          else if (bfd_little_endian (_fd))
            _dis_asm_info.display_endian = BFD_ENDIAN_LITTLE;
          else
            _dis_asm_info.endian = BFD_ENDIAN_UNKNOWN;
          _dis_asm_info.disassembler_options = (char *) "-M intel,intel-mnemonic";
        }


      disassembler_ftype File::getDisassembler()
        {
          if (_dis_asm == nullptr)
            _dis_asm = disassembler(this->_fd);
          return _dis_asm;
        }

      void File::retrieve_symbols()
        {
          long storage_needed;
          long dyn_storage_needed;
          long synthetic_count;
          long number_of_symbols;
          long number_of_dyn_sym;
          size_t i = 0;

          storage_needed = bfd_get_symtab_upper_bound (_fd);
          dyn_storage_needed = bfd_get_dynamic_symtab_upper_bound (_fd);
          if (storage_needed < 0)
            BFP_ASSERT();
          if (storage_needed == 0)
            return;
          if (dyn_storage_needed < 0 && (bfd_get_file_flags (_fd) & DYNAMIC))
            BFP_ASSERT();
          if (dyn_storage_needed < 0)
            BFP_ASSERT();
          if (dyn_storage_needed == 0)
            return;
          symbol_table.set_bytes(
              static_cast<size_t>(storage_needed + dyn_storage_needed));
          if ((number_of_symbols = bfd_canonicalize_symtab (_fd,
                                                            symbol_table)) < 0)
            BFP_ASSERT();
          if ((number_of_dyn_sym = bfd_canonicalize_dynamic_symtab (_fd,
                                                                    symbol_table +
                                                                    number_of_symbols)) <
              0)
            BFP_ASSERT();
          synthetic_count = bfd_get_synthetic_symtab(_fd, number_of_symbols,
                                                     symbol_table,
                                                     number_of_dyn_sym,
                                                     symbol_table +
                                                     number_of_symbols,
                                                     &synthetic_symbol_table);
          symbol_table.resize(
              static_cast<size_t>(synthetic_count + number_of_symbols +
                                  number_of_dyn_sym));
          for (asymbol **_sym = symbol_table.begin() + number_of_dyn_sym +
                                number_of_symbols;
               _sym != symbol_table.end();
               ++_sym, ++i)
            {
              *_sym = synthetic_symbol_table + i;
            }
          ::std::sort(symbol_table.begin(), symbol_table.end(), [](
              const asymbol *_1,
              const asymbol *_2) -> bool
            {
              return _1->value < _2->value;
            });
        }

      ::std::vector<asymbol *> File::get_sym_from_sec(const asection *_sec)
        {
          ::std::vector<asymbol *> _syms;
          auto _pom = ::bfp::filter(symbol_table.begin(), symbol_table.end(),
                                    [&_sec](asymbol *_sym)
                                      {
                                        if (_sym->name == NULL ||
                                            _sym->name[0] == '\0')
                                          return false;
                                        if (_sym->flags &
                                            (BSF_DEBUGGING | BSF_SECTION_SYM))
                                          return false;
                                        if (bfd_is_und_section (
                                                _sym->section) ||
                                            bfd_is_com_section (_sym->section))
                                          return false;
                                        return _sym->section == _sec;
                                      });
          _syms.reserve(_pom.size() + 1);
          if (_sec->symbol != nullptr)
            _syms.push_back(_sec->symbol);
          _syms.insert(_syms.end(), _pom.begin(), _pom.end());
          return _syms;
        }

      long File::getSymTableSize() const
        {
          return symbol_table.size();
        }

      size_t File::getBufferSize() const
        {
          return _buffer.size();
        }
  }