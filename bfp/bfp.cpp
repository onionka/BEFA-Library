/** Binary File Parser
 * @file bfp.cpp
 * @author Miroslav Cibulka
 * @brief Module where Binary File Descriptor is abstracted
 */

#include <bfp.hpp>


namespace BFP
  {
      BFD *BFD::get_unique_instance()
        {
          static BFD *instance = nullptr;
          return instance == nullptr ? instance = new BFD() : instance;
        }

      File *BFD::Open(
          const char *_file_name,
          const char *_target)
        {
          bfd *fd;
          if ((fd = bfd_openr(_file_name, _target)) == NULL)
            return nullptr;
          if (!bfd_check_format(fd, bfd_object))
            {
              bfd_close(fd);
              return nullptr;
            }
          openedFiles.push_back(new File(fd, _file_name, _target));
          return openedFiles.back();
        }

      BFD::BFD()
        {
          bfd_init();
        }

      BFD::~BFD()
        {
          for (auto f : openedFiles)
            delete f;
          openedFiles.clear();
        }

      File::File(
          bfd *fd,
          const char *path,
          const char *target)
          :
          _fd{fd},
          _path{path},
          _target{target}
        {
          long storage_needed;
          long number_of_symbols;
          long i;

          for (asection *_sec = _fd->sections; _sec != NULL; _sec = _sec->next)
            _sections.push_back(new Section(_sec));

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

          for (i = 0; i < number_of_symbols; i++)
            {
              ::std::vector<Section *> _sec_s = ::BFP::search(begin_section(),
                                                              end_section(),
                                                              symbol_table[i]->section);
              _symbols.push_back(new Symbol(symbol_table[i]));
              for (auto _sec : _sec_s)
                {
                  _symbols.back()
                          ->_sections
                          .push_back(_sec);
                  _sec->_symbols
                      .push_back(_symbols.back());
                }
            }
        }

      File::~File()
        {
          for (auto sec : _sections)
            delete sec;
          for (auto sym : _symbols)
            delete sym;
          free(symbol_table);
          bfd_close(_fd);
        }

      const ::std::vector<const char *> BFD::getTargets(const char *_file_name)
        {
          ::std::vector<const char *> _ret;
          bfd *fd;
          for (auto _target : this->_targets)
            if ((fd = bfd_openr(_file_name, _target)) != NULL)
              {
                _ret.push_back(_target);
                bfd_close(fd);
              }
          return _ret;
        }
  }