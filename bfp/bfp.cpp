/** Binary File Parser
 * @file bfp.cpp
 * @author Miroslav Cibulka
 * @brief Module where Binary File Descriptor is abstracted
 */

#include <bfp.hpp>


namespace BFP
  {
      bool Symbol::operator==(
          const Symbol &_compare)
        {
          return (_compare.getValue() == getValue());
        }

      bool Symbol::operator!=(
          const Symbol &_compare)
        {
          return (_compare.getValue() != getValue());
        }

      bool Symbol::operator==(
          const symvalue *_compare)
        {
          return (*_compare == getValue());
        }

      bool Symbol::operator!=(
          const symvalue *_compare)
        {
          return (*_compare != getValue());
        }

      bool Symbol::operator==(
          const asymbol *_ptr)
        {
          return _sym == _ptr;
        }

      bool Symbol::operator!=(
          const asymbol *_ptr)
        {
          return _sym != _ptr;
        }

      bool Symbol::operator==(
          const char *_compare)
        {
          return (::std::string(_compare) == getName());
        }

      bool Symbol::operator!=(
          const char *_compare)
        {
          return (::std::string(_compare) != getName());
        }

      const ::std::string Symbol::getName() const
        {
          return ::std::string(_sym->name);
        }

      const ::std::vector<Section *> Symbol::sections()
        {
          return _sections;
        }

      ::std::vector<Section *>::iterator Symbol::begin_sections()
        {
          return _sections.begin();
        }

      ::std::vector<Section *>::iterator Symbol::end_sections()
        {
          return _sections.end();
        }

      symvalue Symbol::getValue() const
        {
          return _sym->value;
        }

      bool Symbol::hasFlags() const
        {
          return static_cast<bool>(_sym->flags == BSF_NO_FLAGS);
        }

      bool Symbol::isLocal() const
        {
          return static_cast<bool>(_sym->flags & BSF_LOCAL);
        }

      bool Symbol::isGlobal() const
        {
          return static_cast<bool>(_sym->flags & BSF_GLOBAL);
        }

      bool Symbol::isExported() const
        {
          return static_cast<bool>(_sym->flags & BSF_EXPORT);
        }

      bool Symbol::isFunction() const
        {
          return static_cast<bool>(_sym->flags & BSF_FUNCTION);
        }

      bool Symbol::isDebugging() const
        {
          return static_cast<bool>(_sym->flags & BSF_DEBUGGING);
        }

      bool Symbol::isWeak() const
        {
          return static_cast<bool>(_sym->flags & BSF_WEAK);
        }

      bool Symbol::isSectionSymbol() const
        {
          return static_cast<bool>(_sym->flags & BSF_SECTION_SYM);
        }

      bool Symbol::isOldCommon() const
        {
          return static_cast<bool>(_sym->flags & BSF_OLD_COMMON);
        }

      bool Symbol::isNotAtEnd() const
        {
          return static_cast<bool>(_sym->flags & BSF_NOT_AT_END);
        }

      bool Symbol::isInConstructSection() const
        {
          return static_cast<bool>(_sym->flags & BSF_CONSTRUCTOR);
        }

      bool Symbol::isWarning() const
        {
          return static_cast<bool>(_sym->flags & BSF_WARNING);
        }

      bool Symbol::isIndirect() const
        {
          return static_cast<bool>(_sym->flags & BSF_INDIRECT);
        }

      bool Symbol::hasFileName() const
        {
          return static_cast<bool>(_sym->flags & BSF_FILE);
        }

      bool Symbol::isFromDLI() const
        {
          return static_cast<bool>(_sym->flags & BSF_DYNAMIC);
        }

      bool Symbol::hasObjectData() const
        {
          return static_cast<bool>(_sym->flags & BSF_OBJECT);
        }

      Symbol::Symbol(
          asymbol *symbol)
          :
          _sym{symbol}
        { }

      bool Section::operator==(
          const Section &_compare)
        {
          return _compare._sec == _sec;
        }

      bool Section::operator!=(
          const Section &_compare)
        {
          return _compare._sec != _sec;
        }

      bool Section::operator==(
          const asection *_ptr)
        {
          return _sec == _ptr;
        }

      bool Section::operator!=(
          const asection *_ptr)
        {
          return _sec != _ptr;
        }

      bool Section::operator==(
          const char *_compare)
        {
          return ::std::string(_compare) == this->getName();
        }

      bool Section::operator!=(
          const char *_compare)
        {
          return ::std::string(_compare) != this->getName();
        }

      int Section::getIndex() const
        {
          return _sec->index;
        }

      const ::std::string Section::getName() const
        {
          return ::std::string(_sec->name);
        }

      const unsigned char *Section::getContent() const
        {
          return _sec->contents;
        }

      const ::std::vector<alent> Section::getLineNO() const
        {
          return line_numbers;
        }

      ::std::vector<Symbol *>::iterator Section::begin_symbol()
        {
          return this->_symbols
                     .begin();
        }

      ::std::vector<Symbol *>::iterator Section::end_symbol()
        {
          return this->_symbols
                     .end();
        }

      const ::std::vector<Symbol *> Section::symbols()
        {
          return this->_symbols;
        }

      bool Section::hasFlags() const
        {
          return static_cast<bool>(_sec->flags == SEC_NO_FLAGS);
        }

      bool Section::isAllocOnLoad() const
        {
          return static_cast<bool>(_sec->flags & SEC_ALLOC);
        }

      bool Section::isLoadedWithFile() const
        {
          return static_cast<bool>(_sec->flags & SEC_LOAD);
        }

      bool Section::hasRellocInfo() const
        {
          return static_cast<bool>(_sec->flags & SEC_RELOC);
        }

      bool Section::isReadOnly() const
        {
          return static_cast<bool>(_sec->flags & SEC_READONLY);
        }

      bool Section::hasCodeOnly() const
        {
          return static_cast<bool>(_sec->flags & SEC_CODE);
        }

      bool Section::hasDataOnly() const
        {
          return static_cast<bool>(_sec->flags & SEC_DATA);
        }

      bool Section::isInROM() const
        {
          return static_cast<bool>(_sec->flags & SEC_ROM);
        }

      bool Section::hasConstructorInfo() const
        {
          return static_cast<bool>(_sec->flags & SEC_CONSTRUCTOR);
        }

      bool Section::hasContent() const
        {
          return static_cast<bool>(_sec->flags & SEC_HAS_CONTENTS);
        }

      bool Section::isSuppressed() const
        {
          return static_cast<bool>(_sec->flags & SEC_NEVER_LOAD);
        }

      bool Section::isCOFF() const
        {
          return static_cast<bool>(_sec->flags & SEC_COFF_SHARED_LIBRARY);
        }

      bool Section::hasCommonSymbols() const
        {
          return static_cast<bool>(_sec->flags & SEC_IS_COMMON);
        }

      bool Section::isDebugOnly() const
        {
          return static_cast<bool>(_sec->flags & SEC_DEBUGGING);
        }

      bool Section::isInMemory() const
        {
          return static_cast<bool>(_sec->flags & SEC_IN_MEMORY);
        }

      bool Section::isExcluded() const
        {
          return static_cast<bool>(_sec->flags & SEC_EXCLUDE);
        }

      bool Section::isSorted() const
        {
          return static_cast<bool>(_sec->flags & SEC_SORT_ENTRIES);
        }

      bool Section::linkOnce() const
        {
          return static_cast<bool>(_sec->flags & SEC_LINK_ONCE);
        }

      bool Section::linkDuplicates() const
        {
          return static_cast<bool>((_sec->flags & SEC_LINK_DUPLICATES) ==
                                   SEC_LINK_DUPLICATES);
        }

      bool Section::discardDuplicates() const
        {
          return static_cast<bool>((_sec->flags & SEC_LINK_DUPLICATES) ==
                                   SEC_LINK_DUPLICATES_DISCARD);
        }

      bool Section::linkOneDuplicate() const
        {
          return static_cast<bool>((_sec->flags & SEC_LINK_DUPLICATES) ==
                                   SEC_LINK_DUPLICATES_ONE_ONLY);
        }

      bool Section::linkSameSizedDuplicates() const
        {
          return static_cast<bool>((_sec->flags & SEC_LINK_DUPLICATES) ==
                                   SEC_LINK_DUPLICATES_SAME_SIZE);
        }

      bool Section::linkSameDuplicates() const
        {
          return static_cast<bool>((_sec->flags & SEC_LINK_DUPLICATES) ==
                                   SEC_LINK_DUPLICATES_SAME_CONTENTS);
        }

      bool Section::isCreatedByLinker() const
        {
          return static_cast<bool>(_sec->flags & SEC_LINKER_CREATED);
        }

      Section::Section(
          asection *section)
          :
          _sec{section}
        { }

      const char *File::get_path() const
        {
          return _path;
        }

      const char *File::get_target() const
        {
          return _target;
        }

      ::std::vector<Section *>::iterator File::begin_section()
        {
          return this->_sections
                     .begin();
        }

      ::std::vector<Section *>::iterator File::end_section()
        {
          return this->_sections
                     .end();
        }

      ::std::vector<Symbol *>::iterator File::begin_symbol()
        {
          return this->_symbols
                     .begin();
        }

      ::std::vector<Symbol *>::iterator File::end_symbol()
        {
          return this->_symbols
                     .end();
        }

      const ::std::vector<Symbol *> File::symbols()
        {
          return this->_symbols;
        }

      const ::std::vector<Section *> File::sections()
        {
          return this->_sections;
        }
  }

namespace BFP
  {
      BFD *BFD::get_unique_instance()
        {
          static BFD *instance = nullptr;
          return instance == nullptr ? instance = new BFD() : instance;
        }

      File *BFD::Open(
          const ::std::string _file_name,
          const ::std::string _target)
        {
          bfd *fd;
          if ((fd = bfd_openr(_file_name.c_str(), _target.c_str())) == NULL)
            return nullptr;
          if (!bfd_check_format(fd, bfd_object))
            {
              bfd_close(fd);
              return nullptr;
            }
          openedFiles.push_back(
              new File(fd, _file_name.c_str(), _target.c_str()));
          return openedFiles.back();
        }

      BFD::BFD()
        {
          bfd_init();
          for (auto _target = bfd_target_list(); *_target != NULL; ++_target)
            _targets.push_back(::std::string(*_target));
        }

      BFD::~BFD()
        {
          for (auto f : openedFiles)
            delete f;
          openedFiles.clear();
        }
  }

namespace BFP
  {
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

      ::std::vector<::std::string> BFD::getTargets(const ::std::string _file_name)
        {
          ::std::vector<::std::string> _ret;
          bfd *fd;
          for (auto target : _targets)
            if ((fd = bfd_openr(_file_name.c_str(), target.c_str())) != NULL)
              {
                _ret.push_back(target);
                bfd_close(fd);
              }
          return _ret;
        }
  }