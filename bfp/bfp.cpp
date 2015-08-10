/** Binary File Parser
 * @file bfp.cpp
 * @author Miroslav Cibulka
 * @brief Source where Binary File Descriptor is implemented
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
          return getName() == _compare;
        }

      bool Symbol::operator!=(
          const char *_compare)
        {
          return getName() != _compare;
        }

      bool operator==(
          const Symbol &_this,
          const Symbol &_compare)
        {
          return (_compare.getValue() == _this.getValue());
        }

      bool operator!=(
          const Symbol &_this,
          const Symbol &_compare)
        {
          return (_compare.getValue() != _this.getValue());
        }

      bool operator==(
          const Symbol &_this,
          const symvalue *_compare)
        {
          return (*_compare == _this.getValue());
        }

      bool operator!=(
          const Symbol &_this,
          const symvalue *_compare)
        {
          return (*_compare != _this.getValue());
        }

      bool operator==(
          const Symbol &_this,
          const asymbol *_ptr)
        {
          return _this._sym == _ptr;
        }

      bool operator!=(
          const Symbol &_this,
          const asymbol *_ptr)
        {
          return _this._sym != _ptr;
        }

      bool operator==(
          const Symbol &_this,
          const char *_compare)
        {
          return _this.getName() == _compare;
        }

      bool operator!=(
          const Symbol &_this,
          const char *_compare)
        {
          return _this.getName() != _compare;
        }

      const ::std::string Symbol::getName() const
        {
          return ::std::string(_sym->name);
        }

      const Section &Symbol::section()
        {
          return *_section;
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

  }

namespace BFP
  {
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
          return this->getName() == _compare;
        }

      bool Section::operator!=(
          const char *_compare)
        {
          return this->getName() != _compare;
        }

      bool operator==(
          const Section &_this,
          const asection *_ptr)
        {
          return _this._sec == _ptr;
        }

      bool operator!=(
          const Section &_this,
          const asection *_ptr)
        {
          return _this._sec != _ptr;
        }

      bool operator==(
          const Section &_this,
          const char *_compare)
        {
          return _this.getName() == _compare;
        }

      bool operator!=(
          const Section &_this,
          const char *_compare)
        {
          return _this.getName() != _compare;
        }

      bool operator==(
          const Section &_this,
          const Section &_compare)
        {
          return _compare._sec == _this._sec;
        }

      bool operator!=(
          const Section &_this,
          const Section &_compare)
        {
          return _compare._sec != _this._sec;
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


      ::std::vector<Symbol>::const_iterator Section::cbegin()
        {
          return _symbols.cbegin();
        }

      ::std::vector<Symbol>::const_iterator Section::cend()
        {
          return _symbols.cend();
        }

      ::std::vector<Symbol>::const_reverse_iterator Section::crbegin()
        {
          return _symbols.crbegin();
        }

      ::std::vector<Symbol>::const_reverse_iterator Section::crend()
        {
          return _symbols.crend();
        }

      size_t Section::capacity()
        {
          return _symbols.capacity();
        }

      size_t Section::size()
        {
          return _symbols.size();
        }

      size_t Section::max_size()
        {
          return _symbols.max_size();
        }

      Symbol Section::operator[](size_t n)
        {
          return _symbols[n];
        }

      Symbol Section::front()
        {
          return _symbols.front();
        }

      Symbol Section::back()
        {
          return _symbols.back();
        }

      Symbol Section::at(size_t n)
        {
          return _symbols.at(n);
        }

      const Symbol Section::at(size_t n) const
        {
          return _symbols.at(n);
        }

      void Section::pop_back()
        {
          _symbols.pop_back();
        }

      bool Section::empty()
        {
          return _symbols.empty();
        }

      void Section::push_back(Symbol &_sec)
        {
          _symbols.push_back(_sec);
        }

      ::std::vector<Symbol>::iterator Section::begin()
        {
          return _symbols.begin();
        }

      ::std::vector<Symbol>::iterator Section::end()
        {
          return _symbols.end();
        }

      ::std::vector<Symbol>::reverse_iterator Section::rbegin()
        {
          return _symbols.rbegin();
        }

      ::std::vector<Symbol>::reverse_iterator Section::rend()
        {
          return _symbols.rend();
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
  }

namespace BFP
  {
      BFD *BFD::get_unique_instance()
        {
          static BFD *instance = new BFD();
          return instance;
        }

      File *BFD::Open(
          const ::std::string _file_name,
          const ::std::string _target)
        {
          bfd *fd;
          ::std::string _t = _target;
          if (_target == "")
            _t = getTargets(_file_name)[0];
          else if (!checkTarget(_file_name, _target))
            {
              bfd_set_error(bfd_error::bfd_error_invalid_target);
              return nullptr;
            }
          if ((fd = bfd_openr(_file_name.c_str(), _t.c_str())) == NULL)
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
          auto _bfd_targets = bfd_target_list();
          for (auto _target = _bfd_targets; *_target != NULL; ++_target)
            _targets.push_back(::std::string(*_target));
          free(_bfd_targets);
        }

      BFD::~BFD()
        {
          for (auto f : openedFiles)
            delete f;
          openedFiles.clear();
        }

      ::std::vector<::std::string> BFD::getTargets(const ::std::string _file_name)
        {
          ::std::vector<::std::string> _ret;
          for (auto target : _targets)
            if (checkTarget(_file_name, target))
              _ret.push_back(target);
          return _ret;
        }

      bool BFD::checkTarget(
          const ::std::string _file_name,
          const ::std::string _target)
        {
          bfd *fd;
          if ((fd = bfd_openr(_file_name.c_str(), _target.c_str())) != NULL)
            {
              bfd_close(fd);
              return true;
            }
          return false;
        }

      ::std::vector<::std::string> BFD::getAllTargets() const
        {
          return _targets;
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

          _sections.push_back(Section());
          for (asection *_sec = _fd->sections; _sec != NULL; _sec = _sec->next)
            _sections.push_back(Section(_sec));

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

          for (i = 0; i < number_of_symbols; ++i)
            {

              auto _sec = end();
              if (symbol_table[i]->section != nullptr)
                _sec = find(begin(), end(), symbol_table[i]->section);
              if (_sec == end())
                {
                  Symbol _sym(symbol_table[i]);
                  _sym._section = _sections.begin();
                  _sections.begin()->push_back(_sym);
                }
              else
                {
                  Symbol _sym(symbol_table[i]);
                  _sym._section = _sec;
                  _sec->push_back(_sym);
                }
            }
        }

      File::~File()
        {
          bfd_close(_fd);
          free(symbol_table);
        }

      ::std::vector<Section>::const_iterator File::cbegin()
        {
          return _sections.cbegin() + 1;
        }

      ::std::vector<Section>::const_iterator File::cend()
        {
          return _sections.cend();
        }

      ::std::vector<Section>::iterator File::begin()
        {
          return _sections.begin() + 1;
        }

      ::std::vector<Section>::iterator File::end()
        {
          return _sections.end();
        }

      ::std::vector<Section>::const_reverse_iterator File::crbegin()
        {
          return _sections.crbegin();
        }

      ::std::vector<Section>::const_reverse_iterator File::crend()
        {
          return _sections.crend() - 1;
        }

      ::std::vector<Section>::reverse_iterator File::rbegin()
        {
          return _sections.rbegin();
        }

      ::std::vector<Section>::reverse_iterator File::rend()
        {
          return _sections.rend() - 1;
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

      Section File::operator[](size_t n)
        {
          return _sections[n];
        }

      Section File::front()
        {
          return _sections.front();
        }

      Section File::back()
        {
          return _sections.back();
        }

      Section File::at(size_t n)
        {
          return _sections.at(n);
        }

      const Section File::at(size_t n) const
        {
          return _sections.at(n);
        }

      void File::pop_back()
        {
          _sections.pop_back();
        }

      bool File::empty()
        {
          return _sections.empty();
        }

      void File::push_back(Section &_sec)
        {
          _sections.push_back(_sec);
        }
  }