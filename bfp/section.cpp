/**
 * @file section.cpp
 * @author Miroslav Cibulka
 * @brief Section class
 */

#include <bfp.hpp>
#include <algorithm>


namespace bfp
  {
      void Section::sort(bool asc)
        {
          ::std::sort(begin(), end(), [&](
              const Symbol *_1,
              const Symbol *_2) -> bool
            {
              return (asc ? _1->getValue() < _2->getValue() : _1->getValue() >
                                                              _2->getValue()) ||
                     getName() == _1->getName();
            });
        }

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
              bfd_get_section_contents(_parent->_fd, _sec, _data, 0,
                                       getContentSize());
            }
          return _data;
        }

      size_t Section::getContentSize()
        {
          return (size_t) bfd_get_section_size(_sec);
        }

      uint64_t Section::getAddress()
        {
          return bfd_get_section_vma(_parent->_fd, _sec);
        }

      uint64_t Section::getLastAddress()
        {
          return bfd_get_section_vma(_parent->_fd, _sec) + getContentSize();
        }

      uint64_t Section::getNearestAddress(uint64_t _address)
        {
          uint64_t _next_address = 0;
          auto _sym_ite = ::bfp::find(begin(), end(), (symvalue) _address);
          if (_sym_ite == end())
            _next_address = getLastAddress();
          else
            {
              while (_sym_ite != end() && (*_sym_ite)->getValue() == _address)
                _sym_ite++;
              if (_sym_ite == end())
                _next_address = getLastAddress();
              else
                _next_address = (*_sym_ite)->getValue();
            }
          return _next_address;
        }

      uint64_t Section::getNearestPrevAddress(uint64_t _address)
        {
          uint64_t _next_address = 0;
          auto _sym_ite = ::bfp::find(rbegin(), rend(), (symvalue) _address);
          if (_sym_ite == rend())
            _next_address = getAddress();
          else
            {
              while (_sym_ite != rend() && (*_sym_ite)->getValue() == _address)
                _sym_ite++;
              if (_sym_ite == rend())
                _next_address = getAddress();
              else
                _next_address = (*_sym_ite)->getValue();
            }
          return _next_address;
        }

      uint64_t Section::getNearestAddress(Symbol *_sym)
        {
          uint64_t _next_address = 0;
          auto _sym_ite = ::bfp::find(begin(), end(), _sym);
          if (_sym_ite == end())
            _next_address = getLastAddress();
          else
            {
              while (_sym_ite != end() &&
                     (*_sym_ite)->getValue() == _sym->getValue())
                _sym_ite++;
              if (_sym_ite == end())
                _next_address = getLastAddress();
              else
                _next_address = (*_sym_ite)->getValue();
            }
          return _next_address;
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


      Section::__const_iterator Section::cbegin()
        {
          return _symbols.cbegin();
        }

      Section::__const_iterator Section::cend()
        {
          return _symbols.cend();
        }

      Section::__const_reverse_iterator Section::crbegin()
        {
          return _symbols.crbegin();
        }

      Section::__const_reverse_iterator Section::crend()
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

      Section::__data Section::operator[](size_t n)
        {
          return _symbols[n];
        }

      Section::__data Section::front()
        {
          return _symbols.front();
        }

      Section::__data Section::back()
        {
          return _symbols.back();
        }

      Section::__data Section::at(size_t n)
        {
          return _symbols.at(n);
        }

      bool Section::empty()
        {
          return _symbols.empty();
        }

      void Section::push_back(Section::__data _sec)
        {
          _symbols.push_back(_sec);
        }

      Section::__iterator Section::begin()
        {
          return _symbols.begin();
        }

      Section::__iterator Section::end()
        {
          return _symbols.end();
        }

      Section::__reverse_iterator Section::rbegin()
        {
          return _symbols.rbegin();
        }

      Section::__reverse_iterator Section::rend()
        {
          return _symbols.rend();
        }

      Section::Section(
          asection *section,
          File *parent)
          :
          _sec{section},
          _data{nullptr},
          _parent{parent}
        { }

      Section::~Section()
        {
          for (auto _sym : _symbols)
            delete _sym;
          if (_data != nullptr)
            delete[] _data;
        }

      disassemble_info *Section::getDisassembleInfo()
        {
          /*if symbol is not a function this doesn't have any sense*/
          if (!hasContent())
            return nullptr;

          /* This varies so it is set in section (where it varies */
          _parent->_dis_asm_info->buffer = getContent();
          _parent->_dis_asm_info->buffer_vma = getAddress();
          _parent->_dis_asm_info->buffer_length = (unsigned) getContentSize();
          return _parent->_dis_asm_info;
        }

      ::std::vector<Instruction *> Section::getNonSymbolData()
        {
          auto _dis_asm = _parent->getDisassembler();
          auto _dis_asm_info = getDisassembleInfo();
          auto _add = getAddress();
          if (_dis_asm_info == nullptr)
            return _instructions;
          if (_instructions.empty())
            {
              for (int _dis = 0, _instr_size = 0;
                   _add + _dis < getNearestAddress(_add);
                   _dis += _instr_size)
                {
                  _parent->_FFILE.pos = 0;
                  _instr_size = _dis_asm(_add + _dis, _dis_asm_info);
                  if (_instr_size <= 0)
                    break;
                  _instructions.push_back(new Instruction((getContent() + _dis),
                                                          (size_t) _instr_size,
                                                          _parent->_FFILE
                                                                 .buffer,
                                                          _add + _dis));
                }
            }
          return _instructions;
        }
  }