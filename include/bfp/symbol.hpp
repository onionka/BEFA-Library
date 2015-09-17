/**
 * @file symbol.hpp
 * @author Miroslav Cibulka
 * @brief Symbol class
 *
 */

#ifndef __BFP_SYMBOL_HPP
# define __BFP_SYMBOL_HPP

#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif


namespace bfp
  {
      class Section;

      class Instruction;

      class Symbol :
          public Vector<
              ForwardIterator<
                  Instruction,
                  Symbol>>
        {
          /** Section sets instance of this on-fly
           *  so has to have access to private
           *  variables
           */
          friend class Section;

          /** Base type of this class  */
          typedef Vector<
              ForwardIterator<
                  Instruction,
                  Symbol>> _Base;

      public:

          /** Iterator type */
          typedef typename _Base::iterator iterator;

          /** Difference type */
          typedef typename _Base::difference_type difference_type;

          /** Value type (Section) */
          typedef typename _Base::value_type value_type;

          /**
           * @brief this is needed by iterator because he doesn't know
           *        what is next element in row so he invoke this method
           *        that fills instr wrapper and moves offset
           * @param instr is wrapper for instruction
           * @param offset is our pointer to 'vector' that shows
           *        where we are, so we may easily check if iterator
           *        reached end
           */
          void next(
              value_type &instr,
              difference_type &offset);

          ///////////////////////////////////////
          //         Vector operations         //
          ///////////////////////////////////////

          /** begin of iteration */
          iterator begin();

          /** end of iteration - stops at nearest symbol */
          iterator end();

          /** @see size */
          difference_type capacity();

          /** Size is how many bytes is this symbol pointing on */
          difference_type size();

          /** @see size */
          difference_type max_size();

          /** this is unefficient because of looped jump in instructions
           * So time complexity is linear (n).
           */
          value_type operator[](
              int n);

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

          /** @return gets address of symbol/or value */
          symvalue getValue() const;

          uint8_t *getRawData() const;

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

          /** This has to be default/primitive destructor
           *  like constructor so nothing is allocated
           *  via new and nothing needs to be deallocated
           *  (memory-leak-free)
           */
          ~Symbol() = default;

          /** So if we want use this as flyweight object
           * we need this to have default constructor.
           * Parent object sets needed variables on fly.
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
          difference_type _size = -1;

          ::std::function<
              LineInfo(
                  bfd_vma)> _get_line;
        };
  }

#endif //__BFP_SYMBOL_HPP
