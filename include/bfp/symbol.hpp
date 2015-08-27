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
      class File;

      class Section;

      class Symbol
        {
          /** Only File (factory method) may instantiate Symbol class */
          friend class File;

          /** Section may delete file */
          friend class Section;


      public:
          typedef ::elfpp::ForwardIterator<
              Instruction,
              Symbol> __iterator;

          ~Symbol();

          void next(
              Instruction *instr,
              __iterator::difference_type *offset);

          __iterator begin();

          __iterator end();

          __iterator::difference_type capacity();

          __iterator::difference_type size();

          __iterator::difference_type max_size();

          Instruction operator[](
              size_t n);

          /////////////////////////////////////////////
          ///         Comparition operators         ///
          /////////////////////////////////////////////

          bool operator==(
              const Symbol &_compare);

          bool operator!=(
              const Symbol &_compare);

          bool operator==(
              const Symbol *_compare);

          bool operator!=(
              const Symbol *_compare);

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
              const Symbol *_compare);

          friend bool operator!=(
              const Symbol &_this,
              const Symbol *_compare);

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

          /** @return string representation of symbol */
          const ::std::string getName() const;

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

          bool pointsToSection() const;

          bool isOldCommon() const;

          bool isNotAtEnd() const;

          bool isInConstructSection() const;

          bool isWarning() const;

          bool isIndirect() const;

          bool hasFileName() const;

          bool isFromDLI() const;

          bool hasObjectData() const;

          Symbol(Symbol &&_mv)
            {
              ::std::swap(_sym, _mv._sym);
              ::std::swap(_dis_fun, _mv._dis_fun);
              ::std::swap(_dis_info, _mv._dis_info);
              ::std::swap(has_no_intructions, _mv.has_no_intructions);
              ::std::swap(_size, _mv._size);
            }

          Symbol &operator=(Symbol &&_mv)
            {
              ::std::swap(_sym, _mv._sym);
              ::std::swap(_dis_fun, _mv._dis_fun);
              ::std::swap(_dis_info, _mv._dis_info);
              ::std::swap(has_no_intructions, _mv.has_no_intructions);
              ::std::swap(_size, _mv._size);
              return *this;
            }

          /**
           * @brief initialize symbol object
           * @param symbol bfd
           * @param parent File that this symbol belongs to
           */
          Symbol(
              asymbol *symbol,
              disassembler_ftype dis_fun,
              disassemble_info *dis_info);

          Symbol() = default;

          Symbol(const Symbol &_cp)
            {
              _sym = _cp._sym;
              _dis_fun = _cp._dis_fun;
              _dis_info = _cp._dis_info;
              has_no_intructions = _cp.has_no_intructions;
              _size = _cp._size;
            }

          Symbol &operator=(const Symbol &_cp)
            {
              _sym = _cp._sym;
              _dis_fun = _cp._dis_fun;
              _dis_info = _cp._dis_info;
              has_no_intructions = _cp.has_no_intructions;
              _size = _cp._size;
              return *this;
            }

      private:
          /** BFD symbol structure */
          asymbol *_sym = nullptr;
          disassembler_ftype _dis_fun = nullptr;
          disassemble_info *_dis_info = nullptr;

          /** if this symbol has any instructions to disassemble */
          bool has_no_intructions = false;

          __iterator::difference_type _size = -1;
        };
  }

#endif //__BFP_SYMBOL_HPP
