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
          typedef ::bfp::ForwardIterator<
              Instruction,
              Symbol> __iterator;

          ~Symbol();

          void next(
              Instruction *instr,
              __iterator::difference_type *offset);

          ///////////////////////////////////////
          //         Vector operations         //
          ///////////////////////////////////////

          __iterator begin();

          __iterator end();

          __iterator::difference_type capacity();

          __iterator::difference_type size();

          __iterator::difference_type max_size();

          Instruction operator[](
              size_t n);

          ///////////////////////////////////////////
          //         Comparition operators         //
          ///////////////////////////////////////////

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

          /////////////////////////////
          //         GETTERS         //
          /////////////////////////////

          /** @return string representation of symbol */
          const ::std::string getName() const;

          /** @return RAW value of symbol */
          symvalue getValue() const;

          ///////////////////////////////////////////////
          //             Symbol attributes             //
          ///////////////////////////////////////////////

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

          /** So if we want use this as flyweight object
           * we need this to have default constructor
           */
          Symbol() = default;

          /** And move constructor
           * @see Symbol
           */
          Symbol(Symbol &&_mv);

          /** And move operator
           * @see Symbol
           */
          Symbol &operator=(Symbol &&_mv);

          /** And copy constructor
           * @see Symbol
           */
          Symbol(const Symbol &_cp);

          /** And copy operator
           * @see Symbol
           */
          Symbol &operator=(const Symbol &_cp);

          /**
           * @brief initialize symbol object
           * @param symbol bfd
           * @param parent File that this symbol belongs to
           */
          Symbol(
              asymbol *symbol,
              disassembler_ftype dis_fun,
              disassemble_info *dis_info);

      private:
          /** BFD symbol structure */
          asymbol *_sym = nullptr;

          /** BFD disassembler function */
          disassembler_ftype _dis_fun = nullptr;

          /** BFD disassembler info */
          disassemble_info *_dis_info = nullptr;

          /** if this symbol has any instructions to disassemble */
          bool has_no_intructions = false;

          /** size of symbols - bytes */
          __iterator::difference_type _size = -1;
        };
  }

#endif //__BFP_SYMBOL_HPP
