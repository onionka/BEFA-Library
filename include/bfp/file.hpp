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
      public:
          /** Frees all sections and symbols when deleted
           *    By default it is done by BFD so ... don't do it
           */
          ~File();

          /** @return path to this file */
          const char *get_path() const;

          /** @return with which target is this file opened @see BFD::targets */
          const char *get_target() const;

          /////////////////////////////////////
          ///       Vector operations       ///
          /////////////////////////////////////

          ::std::vector<Section>::const_iterator cbegin();

          ::std::vector<Section>::const_iterator cend();

          ::std::vector<Section>::const_reverse_iterator crbegin();

          ::std::vector<Section>::const_reverse_iterator crend();

          ::std::vector<Section>::iterator begin();

          ::std::vector<Section>::iterator end();

          ::std::vector<Section>::reverse_iterator rbegin();

          ::std::vector<Section>::reverse_iterator rend();

          size_t capacity();

          size_t size();

          size_t max_size();

          Section operator[](
              size_t n);

          Section front();

          Section back();

          Section at(
              size_t n);

          bool empty();

      private:
          /// So BFD may create instance of this
          friend class Parser;

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

          /** This should be constant vector so push_back is not allowed (only internal) */
          void push_back(
              Section &_sec);

      private:
          /** File descriptor */
          bfd *_fd;

          /** Path to executable */
          const char *_path;

          /** Target name eg. elf64-x86-64 */
          const char *_target;

          /** Vector of section in file (contains also appropriate symbols) */
          ::std::vector<Section> _sections;

          /** File symbol table */
          asymbol **symbol_table;
        };
  }

#endif //__BFP_FILE_HPP
