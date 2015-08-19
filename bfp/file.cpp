/**
 * @file file.cpp
 * @author Miroslav Cibulka
 * @brief Here is executable File class implemented
 */

#include <bfp.hpp>
#include <algorithm>
#include <cstdarg>


namespace bfp
  {
      File::__ffile File::_FFILE;

      File::File(
          bfd *fd,
          const char *path,
          const char *target)
          :
          _fd{fd},
          _path{path},
          _target{target}
        {
          retrieve_sections();
          retrieve_symbols();
          retrieve_dynamic_symbols();
          //retrieve_synthetic_symbols();
          setDisassembleInfo();
          for (auto &_sec : *this)
            _sec->sort();
        }

      File::~File()
        {
          bfd_close(_fd);
          free(symbol_table);
          free(dynamic_symbol_table);
          //free(synthetic_symbol_table);
          for (auto &_sec: _sections)
            delete _sec;
          if (_dis_asm_info != nullptr)
            delete _dis_asm_info;
        }

      File::__const_iterator File::cbegin()
        {
          return _sections.cbegin();
        }

      File::__const_iterator File::cend()
        {
          return _sections.cend();
        }

      File::__iterator File::begin()
        {
          return _sections.begin();
        }

      File::__iterator File::end()
        {
          return _sections.end();
        }

      File::__const_reverse_iterator File::crbegin()
        {
          return _sections.crbegin();
        }

      File::__const_reverse_iterator File::crend()
        {
          return _sections.crend();
        }

      File::__reverse_iterator File::rbegin()
        {
          return _sections.rbegin();
        }

      File::__reverse_iterator File::rend()
        {
          return _sections.rend();
        }

      size_t File::capacity()
        {
          return _sections.capacity();
        }

      size_t File::size()
        {
          return _sections.size();
        }

      size_t File::max_size()
        {
          return _sections.max_size();
        }

      File::__section File::operator[](size_t n)
        {
          return _sections[n];
        }

      File::__section File::front()
        {
          return _sections.front();
        }

      File::__section File::back()
        {
          return _sections.back();
        }

      File::__section File::at(size_t n)
        {
          return _sections.at(n);
        }

      bool File::empty()
        {
          return _sections.empty();
        }

      void File::push_back(File::__section _sec)
        {
          _sections.push_back(_sec);
        }

      const char *File::get_path() const
        {
          return _path;
        }

      const char *File::get_target() const
        {
          return _target;
        }

      void File::setDisassembleInfo()
        {
          _dis_asm_info = new disassemble_info();
          init_disassemble_info(_dis_asm_info, &_FFILE,
                                (fprintf_ftype) ffprintf);

          /*some bfd settings*/
          _dis_asm_info->flavour = bfd_get_flavour (_fd);
          _dis_asm_info->arch = bfd_get_arch(_fd);
          _dis_asm_info->mach = bfd_get_mach(_fd);
          _dis_asm_info->octets_per_byte = bfd_octets_per_byte(_fd);
          _dis_asm_info->skip_zeroes = 8;
          _dis_asm_info->skip_zeroes_at_end = 3;
          _dis_asm_info->application_data = _fd;

          if (bfd_big_endian (_fd))
            _dis_asm_info->display_endian = BFD_ENDIAN_BIG;
          else if (bfd_little_endian (_fd))
            _dis_asm_info->display_endian = BFD_ENDIAN_LITTLE;
          else
            _dis_asm_info->endian = BFD_ENDIAN_UNKNOWN;

          _dis_asm_info->disassembler_options = (char *) "-M intel,intel-mnemonic";
        }


      disassembler_ftype File::getDisassembler()
        {
          if (_dis_asm == nullptr)
            _dis_asm = disassembler(this->_fd);
          return _dis_asm;
        }

      void File::retrieve_synthetic_symbols()
        {
          synthetic_symbol_table = (asymbol **) malloc(sizeof(asymbol *));
          synthetic_count = bfd_get_synthetic_symtab(_fd, number_of_symbols,
                                                     symbol_table,
                                                     number_of_dyn_sym,
                                                     dynamic_symbol_table,
                                                     synthetic_symbol_table);
          for (long i = 0;
               i < synthetic_count;
               ++i)
            {
              auto _sec = end();
              if ((*synthetic_symbol_table)[i].section != nullptr)
                _sec = ::bfp::find(begin(), end(),
                                   (*synthetic_symbol_table)[i].section);
              if (_sec == end())
                {
                  RAISE(Exception::Parser::WrongFormat);
                }
              else
                {
                  Symbol *_sym = new Symbol((*synthetic_symbol_table) + i,
                                            this);
                  _sym->_section = *_sec;
                  (*_sec)->push_back(_sym);
                }
            }
        }

      void File::retrieve_dynamic_symbols()
        {
          long storage_needed;
          long i;

          storage_needed = bfd_get_dynamic_symtab_upper_bound (_fd);

          if (storage_needed < 0 && (bfd_get_file_flags (_fd) & DYNAMIC))
            BFP_ASSERT();

          if (storage_needed < 0)
            BFP_ASSERT();

          if (storage_needed == 0)
            return;

          dynamic_symbol_table = (asymbol **) malloc(
              static_cast<size_t>(storage_needed));

          number_of_dyn_sym = bfd_canonicalize_dynamic_symtab (_fd,
                                                               dynamic_symbol_table);

          for (i = 0;
               i < number_of_dyn_sym;
               ++i)
            {

              auto _sec = end();
              if (dynamic_symbol_table[i]->section != nullptr)
                _sec = ::bfp::find(begin(), end(),
                                   dynamic_symbol_table[i]->section);
              if (_sec == end())
                {
                  RAISE(Exception::Parser::WrongFormat);
                }
              else
                {
                  Symbol *_sym = new Symbol(dynamic_symbol_table[i], this);
                  _sym->_section = *_sec;
                  (*_sec)->push_back(_sym);
                }
            }
        }

      void File::retrieve_symbols()
        {
          long storage_needed;
          long i;

          storage_needed = bfd_get_symtab_upper_bound (_fd);

          if (storage_needed < 0)
            BFP_ASSERT();

          if (storage_needed == 0)
            return;

          symbol_table = (asymbol **) malloc(
              static_cast<size_t>(storage_needed));

          if ((number_of_symbols = bfd_canonicalize_symtab (_fd,
                                                            symbol_table)) < 0)
            BFP_ASSERT();


          for (i = 0;
               i < number_of_symbols;
               ++i)
            {
              auto _sec = end();
              if (symbol_table[i]->section != nullptr)
                _sec = ::bfp::find(begin(), end(), symbol_table[i]->section);
              if (_sec == end())
                {
                  RAISE(Exception::Parser::WrongFormat);
                }
              else
                {
                  Symbol *_sym = new Symbol(symbol_table[i], this);
                  _sym->_section = *_sec;
                  (*_sec)->push_back(_sym);
                }
            }
        }

      void File::retrieve_sections()
        {
          Section *_s_com = new Section(bfd_com_section_ptr, this);
          Section *_s_und = new Section(bfd_und_section_ptr, this);
          Section *_s_abs = new Section(bfd_abs_section_ptr, this);
          Section *_s_ind = new Section(bfd_ind_section_ptr, this);
          push_back(_s_com);
          push_back(_s_und);
          push_back(_s_abs);
          push_back(_s_ind);
          for (asection *_sec = _fd->sections;
               _sec != NULL;
               _sec = _sec->next)
            {
              Section *_s = new Section(_sec, this);
              push_back(_s);
            }
        }

      int File::ffprintf(
          File::__ffile *f,
          const char *format,
          ...)
        {
          int n;
          va_list args;

          while (1)
            {
              size_t space = f->alloc - f->pos;

              va_start (args, format);
              n = vsnprintf(f->buffer + f->pos, space, format, args);
              va_end (args);

              if (space > (size_t) n)
                break;

              f->realloc(n);
            }
          f->pos += n;

          return n;
        }

      File::FFILE::FFILE()
        {
          buffer = new char[base_size];
          alloc = base_size;
          pos = 0;
        }

      void File::FFILE::realloc(int size)
        {
          alloc = (alloc + size) * 2;
          delete[] buffer;
          buffer = new char[alloc];
          bzero(buffer, alloc);
        }

      File::FFILE::~FFILE()
        {
          delete[] buffer;
          alloc = 0;
          pos = 0;
        }

  }