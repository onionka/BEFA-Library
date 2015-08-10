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
      class Symbol;

      class Section
        {
      public:
          Section(
              const Section &_copy)
            {
              _sec = _copy._sec;
              _symbols = _copy._symbols;
              line_numbers = _copy.line_numbers;
            }

          Section &operator=(
              const Section &_copy)
            {
              _sec = _copy._sec;
              _symbols = _copy._symbols;
              line_numbers = _copy.line_numbers;
              return *this;
            }

          ~Section()
            {
              if (notASection)
                delete _sec;
            }

          /////////////////////////////////////////////
          ///         Comparition operators         ///
          /////////////////////////////////////////////

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

          ///////////////////////////////
          ///         GETTERS         ///
          ///////////////////////////////

          /** What the section number is in the target world */
          int getIndex() const;

          /**
           * @return name of section like .text, .data or .bss
           */
          const ::std::string getName() const;

          /**
           * @returns content in raw format or nullptr if doesn't have any
           */
          const unsigned char *getContent() const;

          /**
           * @return numbers of line
           *         TODO: findout for what
           */
          const ::std::vector <alent> getLineNO() const;


          /////////////////////////////////////
          ///       Vector operations       ///
          /////////////////////////////////////

          ::std::vector<Symbol>::const_iterator cbegin();

          ::std::vector<Symbol>::const_iterator cend();

          ::std::vector<Symbol>::const_reverse_iterator crbegin();

          ::std::vector<Symbol>::const_reverse_iterator crend();

          ::std::vector<Symbol>::iterator begin();

          ::std::vector<Symbol>::iterator end();

          ::std::vector<Symbol>::reverse_iterator rbegin();

          ::std::vector<Symbol>::reverse_iterator rend();

          size_t capacity();

          size_t size();

          size_t max_size();

          Symbol operator[](
              size_t n);

          Symbol front();

          Symbol back();

          Symbol at(
              size_t n);

          bool empty();

          //////////////////////////////////////////////////
          ///             Section attributes             ///
          //////////////////////////////////////////////////

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

          /** This should be constant vector so push_back is not allowed (only internal) */
          void push_back(
              Symbol &_sec);

          /** This cannot be instantiated outside this class
           *    but it is done via File (factory method)
           * @param section is BFD structure that represents
           *          section of binary file ie. .text, .bss, .data
           */
          Section(
              asection *section);

          /** Cannot be instantiated by primitive constructor */
          Section()
              :
              notASection{true}
            {
              _sec = new asection();
              _sec->name = "not_a_section";
            }

      private:
          bool notASection = false;

          /** Section as BFD structure */
          asection *_sec;

          /** Vector of appropriate symbols */
          ::std::vector <Symbol> _symbols;

          /** Vector of line numbers - TODO: findout for what is this */
          ::std::vector <alent> line_numbers;
        };
  }

#endif //__BFP_SECTION_HPP
