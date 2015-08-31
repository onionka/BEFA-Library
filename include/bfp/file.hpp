/**
 * @file file.hpp
 * @author Miroslav Cibulka
 * @brief Here is executable File class implemented
 */

#ifndef __BFP_FILE_HPP
# define __BFP_FILE_HPP

#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif


namespace bfp
  {
      /** Binary file descriptor class
       * @brief has iterators and is instantiated via BFD singleton/factory
       */
      class File :
          public Vector<
              ForwardIterator<
                  Section,
                  File>>
        {
          /** So BFD may create instance of this or delete */
          friend class Parser;

          /** Base type of this class  */
          typedef Vector<
              ForwardIterator<
                  Section,
                  File>> _Base;

          /** Iterator type */
          typedef typename _Base::iterator iterator;

          /** Difference type */
          typedef typename _Base::difference_type difference_type;

          /** Value type (Section) */
          typedef typename _Base::value_type value_type;

      public:
          struct _ffile
            {
              _ffile()
                  :
                  _buffer(42),
                  pos(0)
                { }

              void init()
                {
                  pos = 0;
                  _buffer.zero();
                }

              raw_vector<char> _buffer;
              size_t pos;
            };

          long getSymTableSize() const;

          size_t getBufferSize() const;

          /** @return path to this file */
          ::std::string get_path() const;

          /** @return with which target is this file opened @see BFD::targets */
          ::std::string get_target() const;

          /////////////////////////////////////
          ///       Vector operations       ///
          /////////////////////////////////////

          virtual iterator begin();

          virtual iterator end();

          /**
           * @see Symbol::next
           */
          virtual void next(
              value_type &_sec,
              difference_type &offset);

          virtual difference_type capacity();

          virtual difference_type size();

          virtual difference_type max_size();

          value_type operator[](
              int n);

          /**
           * @details
           *  Frees all sections and symbols when deleted
           *    By default it is done by BFD so ... don't do it
           */
          ~File();

      private:
          /**
           * @details
           *  File can't be created outside of BFD singleton/factory
           *    This is opened by BFD and closed by BFD
           *      (May be accessed via vector of files in BFD)
           * @param fd is binary file descriptor representative
           * @param path to binary file (executable or dynamic library)
           * @param target architecture or format ie. elf64-x86-64
           * @see _target
           */
          File(
              bfd *fd,
              const char *path,
              const char *target);

          /** Cannot be instantiated by primitive constructor */
          File() = delete;

          /** Forbidden copy constructor */
          File(const File &) = delete;

          /** Forbidden move constructor */
          File(File &&) = delete;

          /** Forbidden copy assignment */
          File &operator=(const File &) = delete;

          /** Forbidden move assignment */
          File &operator=(File &&) = delete;

          /**
           * @brief Our custom fake printf that stores output from disassembler
           * @param f our fake file
           * @param format string
           * @param ... formating info
           * @return number of written bytes to f (FFILE)
           */
          ATTRIBUTE_PRINTF_2 static int ffprintf(
              _ffile *f,
              const char *format,
              ...);

          /** @brief This prepares disassembler */
          disassembler_ftype getDisassembler();

          /** @return vector of symbols that belongs to section */
          ::std::vector<asymbol *> get_sym_from_sec(const asection *sec);

          /** reads symbols from file */
          void retrieve_symbols();

          /**
           * @brief Prepares structure where disassembler will store info
           *        This is here because things that are allocated and initialized
           *        here are used by all symbols so they only
           * @return pointer to that structure
           */
          void setDisassembleInfo();

      private:

          /** File descriptor */
          bfd *_fd;

          /** Path to executable */
          ::std::string _path;

          /** Target name ie. elf64-x86-64 */
          ::std::string _target;

          /** File symbol table */
          raw_vector<asymbol *> symbol_table;

          /** Synthetic symbol table (extra symbols?) */
          asymbol *synthetic_symbol_table;
          long number_of_symbols;
          long number_of_dyn_sym;
          long synthetic_count;
          long table_count;

          /** Function that disassembles binary file */
          disassembler_ftype _dis_asm = 0;

          /** Here will be all info about instructions */
          disassemble_info _dis_asm_info;

          /** Binary content of this file */
          raw_vector<uint8_t> _buffer;

          /** Static file that stores everything that will be written to it
           *  so we may access data later and clear it
           */
          static _ffile _FFILE;
        };
  }

#endif //__BFP_FILE_HPP
