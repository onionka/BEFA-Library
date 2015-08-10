/** Binary File Parser
 * @file section.cpp
 * @author Miroslav Cibulka
 * @brief Section class
 */

#include <bfp.hpp>

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
  }