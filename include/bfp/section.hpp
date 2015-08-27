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

      class Instruction;

      class Section
        {
          /** Only File may instantiate this */
          friend class File;

          friend class Symbol;

      public:
          typedef ::elfpp::ForwardIterator<
              Symbol,
              Section> __iterator;

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
          Section::__iterator::difference_type getContentSize();

          /**
           * @return address of section in memory
           */
          uint64_t getAddress();

          /**
           * @return farthest possible address inside this section
           */
          uint64_t getLastAddress();

          /**
           * @return numbers of line
           */
          const ::std::vector<alent> getLineNO() const;


          // ***************************** //
          //       Vector operations       //
          // ***************************** //

          __iterator begin();

          __iterator end();

          __iterator::difference_type capacity();

          __iterator::difference_type size();

          __iterator::difference_type max_size();

          void next(
              Symbol *_sym,
              __iterator::difference_type *offset);

          Symbol operator[](
              size_t n);

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
          /** frees memory -> only File may do it */
          ~Section();

          /** Forbidden primitive constructor */
          Section() = delete;

          /** Forbidden copy constructor */
          Section(
              const Section &) = delete;

          /** Forbidden move constructor */
          Section(Section &&) = delete;

          /** Forbidden copy assignment */
          Section &operator=(const Section &) = delete;

          /** Forbidden move assignment */
          Section &operator=(Section &&) = delete;

          /**
           * @brief Prepares structure where disassembler will store info
           *        This is here because things that are allocated and initialized
           *        here are used by all symbols so they only
           * @return pointer to that structure
           */
          disassemble_info *getDisassembleInfo();

          /** This cannot be instantiated outside this class
           *    but it is done via File (factory method)
           * @param section is BFD structure that represents
           *          section of binary file ie. .text, .bss, .data
           */
          Section(
              asection *section,
              bfd *bfd,
              disassembler_ftype dis_asm,
              disassemble_info dis_info,
              ::std::vector<asymbol *> &&symbols);

      private:
          /** Section as BFD structure */
          asection *_sec;

          /** Vector of line numbers - TODO: findout for what is this */
          ::std::vector<alent> _line_numbers;

          /** Binary content of this file */
          uint8_t *_data = nullptr;

          /** Number of symbols */
          int64_t _size = -1;

          /** BFD structures: */
          disassembler_ftype _dis_asm;
          disassemble_info _dis_info;
          ::std::vector<asymbol *> _symbols;
          bfd *_bfd;
        };
  }

#endif //__BFP_SECTION_HPP
