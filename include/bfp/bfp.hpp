/**
 * @file bfp.hpp
 * @author Miroslav Cibulka
 * @brief This is module in which is described Binary File Descriptor as class
 *        and his structures with operations.
 *        @code
 *          #include <iostream>
 *          #include <bfp.hpp>
 *          
 *          
 *          int main(
 *              int args,
 *              const char **argv)
 *            {
 *              auto _bfd = ::BFP::BFD::get_unique_instance();
 *              try
 *                {
 *                  auto file = _bfd->Open(*argv, "");
 *                  if (file == nullptr)
 *                    {
 *                      BFP_ASSERT();
 *                      return EXIT_FAILURE;
 *                    }
 *                  for (auto &_sec : *file)
 *                    for (auto &_sym : _sec)
 *                      {
 *                        /// process symbol
 *                      }
 *                } catch (::BFP::base_exception &ex)
 *                {
 *                  ::std::cerr << ex.what();
 *                  return EXIT_FAILURE;
 *                }
 *              delete _bfd;
 *              return EXIT_SUCCESS;
 *            }
 *        @endcode
 *        TODO: detailed description
 */

#ifndef __BFP_BFP_HPP
#define __BFP_BFP_HPP

#include <vector>                 /// ::std::vector<class>
#include <iterator>               /// ::std::iterator
#include <map>                    /// ::std::map
#include <bfp.hpp>                /// additional includes from project
#include <string>                 /// ::std::to_string, ::std::string
#include <type_traits>            /// ::std::remove_reference
#include <bfp/support_helpers.tpp>/// support template functions


#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif

namespace BFP
  {
      /** Derefer pointer to object 'till only object remain
       * @param __ptr pointer type
       * @param _obj object/pointer to object
       * @return reference to object
       */
      template<
          typename __ptr>
        typename remove_references<__ptr>::type &dereference(
            __ptr *_obj);


      /** Finds elements via comparing not the same typed value,
       *      but has to have implemented operator == with __value type
       * @param __ite iterator type
       * @param __finder_type is finding value type
       * @param __value_type MUST be pointer or has implemented dereference operator
       * @param _begin iterator
       * @param _end iterator
       * @param _val to be search for
       * @return vector of found elements
       */
      template<
          typename __ite,
          typename __finder_type,
          typename __value = typename __ite::value_type>
        const ::std::vector<__value> search(
            __ite _begin,
            __ite _end,
            __finder_type _val);

      /** Finds element via comparing not the same typed value,
       *      but has to have implemented operator == with __value type
       * @param __ite iterator type
       * @param __finder_type is finding value type
       * @param __value_type MUST be pointer or has implemented dereference operator
       * @param _begin iterator
       * @param _end iterator
       * @param _val to be search for
       * @return iterator on first element found
       */
      template<
          typename __ite,
          typename __finder_type,
          typename __value = typename __ite::value_type>
        __ite find(
            __ite _begin,
            __ite _end,
            __finder_type _val);

      /**
        * @brief Apply function to range from begin to end
        * @param begin iterator
        * @param end iterator
        * @param func is function with parameters
        *        @code
        *          func(__value _val, __args... _args);
        *        @endcode
        * @param _args are variadic of func arguments
        * @param __ite class type
        * @param __func class type
        * @param __args Variadic arguments
        * @param __value type of iterator
        */
      template<
          typename __ite,
          typename __func,
          typename __ret = typename __apply::function_traits<__func>::result_type,
          typename __value = typename __ite::value_type,
          typename... __args>
        __ret apply(
            __ite begin,
            __ite end,
            __func func,
            __args... _args);


      /**
       * @brief Filters out range between begin and end via functional conditions
       * @code
       *       auto _filtered = ::BFP::filter(_section.begin(), _section.end(),
       *                             [](::BFP::Section *_sec) -> bool
       *                               {
       *                                 return _sec->getName() == ".text";
       *                               },
       *                             [](::BFP::Section *_sec) -> bool
       *                               {
       *                                 return true;
       *                               });
       * @endcode
       * @param __ite Iterator type
       * @param __funcs Function type - everything with operator()
       * @param begin iterator
       * @param end iterator
       * @param _func are variadic functions as conditions
       *              - has to return boolean true/false
       */
      template<
          class __ite,
          typename... __funcs>
        ::std::vector<typename __ite::value_type> filter(
            __ite begin,
            __ite end,
            __funcs... _func);

      class Section;

      class Symbol;

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
              const symvalue *_compare);

          bool operator!=(
              const symvalue *_compare);

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
              const symvalue *_compare);

          friend bool operator!=(
              const Symbol &_this,
              const symvalue *_compare);

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
          const ::std::vector<alent> getLineNO() const;


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

          const Symbol at(
              size_t n) const;

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

          /** This should be constant vector so pop_back is not allowed (only internal) */
          void pop_back();

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
          ::std::vector<Symbol> _symbols;

          /** Vector of line numbers - TODO: findout for what is this */
          ::std::vector<alent> line_numbers;
        };

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

          const Section at(
              size_t n) const;

          bool empty();

      private:
          /// So BFD may create instance of this
          friend class BFD;

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

          /** This should be constant vector so pop_back is not allowed (only internal) */
          void pop_back();

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

      /** Singleton of Binary File Descriptor
       * @brief This represents BFD class.
       *        Instance is static so there will be only one per runtime.
       *        <tt>get_unique_instance</tt> returns the same instance every time it is called.
       * @example tests/TestProgram/main.cpp
       */
      class BFD
        {
      public:
          /** Get unique instance of BFD
           *    (may be called multiple times but same object is returned)
           * @return static instance of BFD
           * @see BFD
           */
          static BFD *get_unique_instance();

          /** Factory method that opens file with _file_name and _target
           * @param _file_name is name of file that will be loaded instantly.
           *                   It is loaded at the time of opening 'cause bfd is implemented
           *                   that way (it gets all symbols at once).
           * @param _target is target architecture for instance 64-bit elf format is "elf64-x86-64".
           *                For list of possible targets call <tt>getTargetsList()</tt>.
           *                If this is "" (empty), first of possible targets is chosen
           * @return instance of <tt>File</tt> that will be automaticly deallocate on deletion BFD instance
           */
          File *Open(
              ::std::string _file_name,
              ::std::string _target);

          /**
           * @param _file_name is name of tested file
           * @return all targets that may be used for file
           */
          ::std::vector<::std::string> getTargets(
              ::std::string _file_name);

          ::std::vector<::std::string> getAllTargets() const;

          /**
           * @brief Checks if target is appropriate to file
           * @param _file_name is name of executable that we are checking
           * @param _target is tested target
           * @return true if _target is appropriate
           */
          bool checkTarget(
              const ::std::string _file_name,
              const ::std::string _target);

          /** Closes all opened Files in vector openedFiles */
          ~BFD();

      private:
          /** It is forbidden to create instance anywhere but in this class only
           *    (once at the beginning)
           */
          BFD();

          /** Forbidden copy constructor */
          BFD(const BFD &) = delete;

          /** Forbidden move constructor */
          BFD(BFD &&) = delete;

          /** Forbidden copy assignment */
          BFD &operator=(const BFD &) = delete;

          /** Forbidden move assignment */
          BFD &operator=(BFD &&) = delete;

      public:
          /** This is public so everyone may use this
           *      Feel free to deallocate whatever file when needed but has to be
           *      erased from this vector afterwards (SIGSEGV - double free/delete)
           */
          ::std::vector<File *> openedFiles;

          ::std::vector<::std::string> errors;

      private:
          /** vector of all possible targets */
          ::std::vector<::std::string> _targets;
        };
  }

#include <bfp/support.tpp>        /// support template functions


#endif // __BFP_BFP_HPP
