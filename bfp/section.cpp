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
          if (_data == nullptr)
            {
              /*we need section data to disassemble*/
              _data = new uint8_t[getContentSize()];
              bfd_get_section_contents(_bfd, _sec, _data, 0,
                                       static_cast<size_t>(getContentSize()));
            }
          return _data;
        }

      Section::__iterator::difference_type Section::getContentSize()
        {
          return bfd_get_section_size(_sec);
        }

      uint64_t Section::getAddress()
        {
          return bfd_get_section_vma(_bfd, _sec);
        }

      uint64_t Section::getLastAddress()
        {
          return bfd_get_section_vma(_bfd, _sec) + getContentSize();
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

      Section::__iterator::difference_type Section::capacity()
        {
          return size();
        }

      Section::__iterator::difference_type Section::size()
        {
          return _symbols.size();
        }

      Section::__iterator::difference_type Section::max_size()
        {
          return size();
        }

      void Section::next(
          Symbol *_sym,
          Section::__iterator::difference_type *offset)
        {
          (*offset) += 1;
          _sym->_dis_fun = _dis_asm;
          _sym->_dis_info = getDisassembleInfo();
          _sym->_sym = _symbols[*offset];
          if ((*offset) + 1 < size())
            {
              for (auto _s: _symbols)
                if (bfd_asymbol_value(_s) > _sym->getValue())
                  {
                    _sym->_size = bfd_asymbol_value(_s) - _sym->getValue();
                    break;
                  }
            }
          else
            _sym->_size = hasContent() ? getContentSize() : 0;
          _sym->has_no_intructions = false;
        }

      Symbol Section::operator[](size_t n)
        {
          __iterator _ite = begin();
          for (size_t i = 0;
               i < n;
               i++, _ite++);
          return *_ite;
        }

      Section::__iterator Section::begin()
        {
          __iterator _ite(this, 0);
          if (size() <= 0)
            return end();
          _ite->_sym = _symbols.front();
          _ite->_dis_fun = _dis_asm;
          _ite->_dis_info = getDisassembleInfo();
          if (size() > 1)
            {
              /* We are finding next nearest possible address *
               * that is not the same with created symbol     */
              for (auto _s: _symbols)
                if (bfd_asymbol_value(_s) > _ite->getValue())
                  {
                    _ite->_size = bfd_asymbol_value(_s) - _ite->getValue();
                    break;
                  }
            }
          else
            _ite->_size = getContentSize();
          _ite->has_no_intructions = false;
          return _ite;
        }

      Section::__iterator Section::end()
        {
          return __iterator(this, size() - 1);
        }

      Section::Section(
          asection *section,
          bfd *bfd,
          disassembler_ftype dis_asm,
          disassemble_info dis_info,
          ::std::vector<asymbol *> &&symbols)
          :
          _sec(section),
          _dis_asm(dis_asm),
          _dis_info(dis_info),
          _symbols(symbols),
          _bfd(bfd)
        { }

      Section::~Section()
        {
          if (_data != nullptr)
            delete[] _data;
        }

      disassemble_info *Section::getDisassembleInfo()
        {
          /* This varies so it is set in section (where it varies */
          _dis_info.buffer = getContent();
          _dis_info.buffer_vma = getAddress();
          _dis_info.buffer_length = (unsigned) getContentSize();
          return &_dis_info;
        }
  }