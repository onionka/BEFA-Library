/**
 * @file section.hpp
 * @author Miroslav Cibulka
 * @brief Section class
 */


#ifndef __BFP_SECTION_HPP
# define __BFP_SECTION_HPP

#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif


namespace bfp
  {
      class File;

      class Symbol;

      class Section
        {
      public:
          typedef Symbol *__symbol;
          typedef ::std::vector<__symbol> __sym_vector;
          typedef __sym_vector::iterator __iterator;
          typedef __sym_vector::reverse_iterator __reverse_iterator;
          typedef __sym_vector::const_iterator __const_iterator;
          typedef __sym_vector::const_reverse_iterator __const_reverse_iterator;

          ~Section();

          // ************************************* //
          //         Comparition operators         //
          // ************************************* //

          bool operator==(
              const Section &_compare);

          bool operator!=(
              const Section &_compare);

          bool operator==(
              const asection *_ptr);

          bool operator!=(
              const asection *_ptr);

          bool operator==(
              const char *_compare);

          bool operator!=(
              const char *_compare);

          friend bool operator==(
              const Section &_this,
              const Section &_compare);

          friend bool operator!=(
              const Section &_this,
              const Section &_compare);

          friend bool operator==(
              const Section &_this,
              const asection *_ptr);

          friend bool operator!=(
              const Section &_this,
              const asection *_ptr);

          friend bool operator==(
              const Section &_this,
              const char *_compare);

          friend bool operator!=(
              const Section &_this,
              const char *_compare);

          // *********************** //
          //         GETTERS         //
          // *********************** //

          /** What the section number is in the target world */
          int getIndex() const;

          /**
           * @return name of section like .text, .data or .bss
           */
          const ::std::string getName() const;

          /**
           * @return binary content (lazy read)
           */
          uint8_t *getContent();

          /**
           * @return size of binary content
           */
          size_t getContentSize();

          /**
           * @return address of section in memory
           */
          uint64_t getAddress();

          /**
           * @return farthest possible address inside this section
           */
          uint64_t getLastAddress();

          /**
           * @return next nearest address to the symbol in parameter
           */
          uint64_t getNearestAddress(Symbol *_sym);

          /**
           * @return numbers of line
           */
          const ::std::vector<alent> getLineNO() const;


          // ***************************** //
          //       Vector operations       //
          // ***************************** //

          __const_iterator cbegin();

          __const_iterator cend();

          __const_reverse_iterator crbegin();

          __const_reverse_iterator crend();

          __iterator begin();

          __iterator end();

          __reverse_iterator rbegin();

          __reverse_iterator rend();

          size_t capacity();

          size_t size();

          size_t max_size();

          __symbol operator[](
              size_t n);

          __symbol front();

          __symbol back();

          __symbol at(
              size_t n);

          bool empty();

          // ******************************************** //
          //              Section attributes              //
          // ******************************************** //

          bool hasFlags() const;

          bool isAllocOnLoad() const;

          bool isLoadedWithFile() const;

          bool hasRellocInfo() const;

          bool isReadOnly() const;

          bool hasCodeOnly() const;

          bool hasDataOnly() const;

          bool isInROM() const;

          bool hasConstructorInfo() const;

          bool hasContent() const;

          bool isSuppressed() const;

          bool isCOFF() const;

          bool hasCommonSymbols() const;

          bool isDebugOnly() const;

          bool isInMemory() const;

          bool isExcluded() const;

          bool isSorted() const;

          bool linkOnce() const;

          bool linkDuplicates() const;

          bool discardDuplicates() const;

          bool linkOneDuplicate() const;

          bool linkSameSizedDuplicates() const;

          bool linkSameDuplicates() const;

          bool isCreatedByLinker() const;

      private:
          /// Only File may instantiate this
          friend class File;

          /** Forbidden primitive constructor */
          Section() = delete;

          /** Forbidden copy constructor */
          Section(const Section &) = delete;

          /** Forbidden move constructor */
          Section(Section &&) = delete;

          /** Forbidden copy assignment */
          Section &operator=(const Section &) = delete;

          /** Forbidden move assignment */
          Section &operator=(Section &&) = delete;

          /** This should be constant vector so push_back is not allowed (only internal) */
          void push_back(
              __symbol _sec);

          /** This cannot be instantiated outside this class
           *    but it is done via File (factory method)
           * @param section is BFD structure that represents
           *          section of binary file ie. .text, .bss, .data
           */
          Section(
              asection *section,
              File *parent);

      private:
          bool _not_a_section = false;

          /** Section as BFD structure */
          asection *_sec;

          /** Vector of appropriate symbols */
          __sym_vector _symbols;

          /** Vector of line numbers - TODO: findout for what is this */
          ::std::vector<alent> _line_numbers;

          /** Binary content of this file */
          uint8_t *_data;

          /** File where Section belongs */
          File *_parent;
        };
  }

#endif //__BFP_SECTION_HPP
