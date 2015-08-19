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

#include <vector>                 /// ::std::vector<class>
#include <dis-asm.h>
#include <iterator>               /// ::std::iterator
#include <map>                    /// ::std::map
#include <string>                 /// ::std::to_string, ::std::string


namespace bfp
  {
      /** Binary file descriptor class
       * @brief has iterators and is instantiated via BFD singleton/factory
       */
      class File
        {
          /** So BFD may create instance of this or delete */
          friend class Parser;

          /** For accessing private data */
          friend class Section;

          /** For accessing private data */
          friend class Symbol;

      public:
          typedef Section *__data;
          typedef ::std::vector<__data> __sec_vector;
          typedef __sec_vector::iterator __iterator;
          typedef __sec_vector::reverse_iterator __reverse_iterator;
          typedef __sec_vector::const_iterator __const_iterator;
          typedef __sec_vector::const_reverse_iterator __const_reverse_iterator;
          typedef struct FFILE
            {
              const size_t base_size = 64;
              char *buffer = (char *) "";
              size_t pos;
              size_t alloc;

              FFILE();

              /** Reallocates _buffer with desire size*/
              void realloc(int size);

              ~FFILE();

            } __ffile;

          /** @return path to this file */
          const char *get_path() const;

          /** @return with which target is this file opened @see BFD::targets */
          const char *get_target() const;

          /////////////////////////////////////
          ///       Vector operations       ///
          /////////////////////////////////////

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

          __data operator[](
              size_t n);

          __data front();

          __data back();

          __data at(
              size_t n);

          bool empty();

          /** Frees all sections and symbols when deleted
           *    By default it is done by BFD so ... don't do it
           */
          ~File();

      private:
          /** File can't be created outside of BFD singleton/factory
           *    This is opened by BFD and closed by BFD
           *      (May be accessed via vector of files in BFD)
           *  @param fd is binary file descriptor representative
           *  @param path to binary file (executable or dynamic library)
           *  @param target architecture or format ie. elf64-x86-64
           *  @see _target
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

          /** This should be constant vector so push_back is not allowed (only internal) */
          void push_back(
              __data _sec);

          /**
           * @brief Our custom fake printf that stores output from disassembler
           * @param f our fake file
           * @param format string
           * @param ... formating info
           * @return number of written bytes to f (FFILE)
           */
          ATTRIBUTE_PRINTF_2
          static int ffprintf(
              __ffile *f,
              const char *format,
              ...);

          /**
           * @brief This prepares disassembler
           */
          disassembler_ftype getDisassembler();

          /** reads dynamic symbols if there are any */
          void retrieve_dynamic_symbols();

          /** reads sections from file */
          void retrieve_sections();

          /** reads symbols from file */
          void retrieve_symbols();

          void retrieve_synthetic_symbols();

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
          const char *_path;

          /** Target name ie. elf64-x86-64 */
          const char *_target;

          /** Vector of section in file (contains also appropriate symbols) */
          __sec_vector _sections;

          /** File symbol table */
          asymbol **symbol_table;
          long number_of_symbols;

          asymbol **dynamic_symbol_table;
          long number_of_dyn_sym;

          asymbol **synthetic_symbol_table;
          long synthetic_count;

          /** Function that disassembles binary file */
          disassembler_ftype _dis_asm = nullptr;

          /** Here will be all info about instructions */
          disassemble_info *_dis_asm_info = nullptr;

          /** Static file that stores everything that will be written to it
           *  so we may access data later and clear it
           */
          static __ffile _FFILE;
        };
  }

#endif //__BFP_FILE_HPP
