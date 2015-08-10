/**
 * @file symbol.hpp
 * @author Miroslav Cibulka
 * @brief Symbol class
 */

#ifndef __BFP_SYMBOL_HPP
# define __BFP_SYMBOL_HPP

#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif


namespace bfp
  {
      class Symbol
        {
      public:

          Symbol(
              const Symbol &_copy)
            {
              _section = _copy._section;
              _sym = _copy._sym;
            }

          Symbol &operator=(
              const Symbol &_copy)
            {
              _section = _copy._section;
              _sym = _copy._sym;
              return *this;
            }

          /////////////////////////////////////////////
          ///         Comparition operators         ///
          /////////////////////////////////////////////

          bool operator==(
              const Symbol &_compare);

          bool operator!=(
              const Symbol &_compare);

          bool operator==(
              const symvalue _compare);

          bool operator!=(
              const symvalue _compare);

          bool operator==(
              const asymbol *_ptr);

          bool operator!=(
              const asymbol *_ptr);

          bool operator==(
              const char *_compare);

          bool operator!=(
              const char *_compare);


          friend bool operator==(
              const Symbol &_this,
              const Symbol &_compare);

          friend bool operator!=(
              const Symbol &_this,
              const Symbol &_compare);

          friend bool operator==(
              const Symbol &_this,
              const symvalue _compare);

          friend bool operator!=(
              const Symbol &_this,
              const symvalue _compare);

          friend bool operator==(
              const Symbol &_this,
              const asymbol *_ptr);

          friend bool operator!=(
              const Symbol &_this,
              const asymbol *_ptr);

          friend bool operator==(
              const Symbol &_this,
              const char *_compare);

          friend bool operator!=(
              const Symbol &_this,
              const char *_compare);

          ///////////////////////////////
          ///         GETTERS         ///
          ///////////////////////////////

          const ::std::string getName() const;

          /** @return all sections where this symbol is (RO) - may be empty*/
          const Section &section();

          /** @return RAW value of symbol */
          symvalue getValue() const;

          /////////////////////////////////////////////////
          ///             Symbol attributes             ///
          /////////////////////////////////////////////////

          bool hasFlags() const;

          bool isLocal() const;

          bool isGlobal() const;

          bool isExported() const;

          bool isFunction() const;

          bool isDebugging() const;

          bool isWeak() const;

          bool isSectionSymbol() const;

          bool isOldCommon() const;

          bool isNotAtEnd() const;

          bool isInConstructSection() const;

          bool isWarning() const;

          bool isIndirect() const;

          bool hasFileName() const;

          bool isFromDLI() const;

          bool hasObjectData() const;

      private:
          /// Only File (factory method) may instantiate Symbol class
          friend class File;

          Symbol(
              asymbol *symbol);

          /** Cannot be instantiated by primitive constructor */
          Symbol() = delete;

      private:
          /** Appropriate Section to this symbol */
          ::std::vector<Section>::iterator _section;

          /** BFD symbol structure */
          asymbol *_sym;
        };
  }

#endif //__BFP_SYMBOL_HPP
