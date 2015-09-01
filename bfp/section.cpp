/**
 * @file section.cpp
 * @author Miroslav Cibulka
 * @brief Section class
 */

#include <bfp.hpp>
#include <algorithm>


namespace bfp
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

      uint8_t *Section::getContent()
        {
          return _data;
        }

      size_t Section::getContentSize()
        {
          return bfd_get_section_size(_sec);
        }

      uint64_t Section::getAddress()
        {
          return _sec->vma;
        }

      uint64_t Section::getLastAddress()
        {
          return _sec->vma + getContentSize();
        }

      const ::std::vector<alent> Section::getLineNO() const
        {
          return _line_numbers;
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

      Section::difference_type Section::capacity()
        {
          return size();
        }

      Section::difference_type Section::size()
        {
          return _symbols.size();
        }

      Section::difference_type Section::max_size()
        {
          return size();
        }

      void Section::next(
          Symbol &_sym,
          Section::difference_type &offset)
        {
          if ((offset += 1) == size())
            return;
          _sym._dis_fun = _dis_asm;
          _sym._dis_info = getDisassembleInfo();
          _sym._sym = _symbols[offset];
          if (offset + 1 < size())
            {
              /* We are finding next nearest possible address *
               * that is not the same with created symbol     */
              auto _ite = _symbols.begin() + offset + 1;
              while (_ite != _symbols.end() &&
                     bfd_asymbol_value(*_ite) == _sym.getValue())
                _ite++;
              if (_ite == _symbols.end())
                _sym._size = getContentSize();
              else
                _sym._size = bfd_asymbol_value(*_ite) - _sym.getValue();
            }
          else
            _sym._size = hasContent() ? getContentSize() : 0;
          _sym.has_no_intructions = false;
        }

      Symbol Section::operator[](int n)
        {
          iterator _ite = begin();
          for (int i = 0;
               i < n;
               i++, _ite++);
          return *_ite;
        }

      Section::iterator Section::begin()
        {
          iterator _ite(this, 0);
          if (size() <= 0)
            return end();
          _ite->_sym = _symbols.front();
          _ite->_dis_fun = _dis_asm;
          _ite->_dis_info = getDisassembleInfo();
          if (size() > 1)
            {
              /* We are finding next nearest possible address *
               * that is not the same with created symbol     */
              auto _i = _symbols.begin() + 1;
              while (_i != _symbols.end() &&
                     bfd_asymbol_value(*_i) == _ite->getValue())
                _i++;
              if (_i == _symbols.end())
                _ite->_size = getContentSize();
              else
                _ite->_size = bfd_asymbol_value(*_i) - _ite->getValue();
            }
          else
            _ite->_size = getContentSize();
          _ite->has_no_intructions = false;
          return _ite;
        }

      Section::iterator Section::end()
        {
          return iterator(this, size());
        }

      Section::Section(
          asection *section,
          disassembler_ftype dis_asm,
          disassemble_info *dis_info,
          ::std::vector<asymbol *> &&symbols)
          :
          _sec(section),
          _dis_asm(dis_asm),
          _dis_info(dis_info),
          _symbols(symbols)
        { }

      Section::~Section()
        { }

      disassemble_info *Section::getDisassembleInfo()
        {
          /* This varies so it is set in section (where it varies */
          _dis_info->buffer = getContent();
          _dis_info->buffer_vma = getAddress();
          _dis_info->buffer_length = (unsigned) getContentSize();
          return _dis_info;
        }

      Section::Section(
          const Section &_cp)
        {
          _sec = _cp._sec;
          _line_numbers = _cp._line_numbers;
          _data = _cp._data;
          _dis_asm = _cp._dis_asm;
          _dis_info = _cp._dis_info;
          _symbols = _cp._symbols;
        }

      Section::Section(Section &&_mv)
        {
          ::std::swap(_sec, _mv._sec);
          ::std::swap(_line_numbers, _mv._line_numbers);
          ::std::swap(_data, _mv._data);
          ::std::swap(_dis_asm, _mv._dis_asm);
          ::std::swap(_dis_info, _mv._dis_info);
          ::std::swap(_symbols, _mv._symbols);
        }

      Section &Section::operator=(const Section &_cp)
        {
          _sec = _cp._sec;
          _line_numbers = _cp._line_numbers;
          _data = _cp._data;
          _dis_asm = _cp._dis_asm;
          _dis_info = _cp._dis_info;
          _symbols = _cp._symbols;
          return *this;
        }

      Section &Section::operator=(Section &&_mv)
        {
          ::std::swap(_sec, _mv._sec);
          ::std::swap(_line_numbers, _mv._line_numbers);
          ::std::swap(_data, _mv._data);
          ::std::swap(_dis_asm, _mv._dis_asm);
          ::std::swap(_dis_info, _mv._dis_info);
          ::std::swap(_symbols, _mv._symbols);
          return *this;
        }
  }