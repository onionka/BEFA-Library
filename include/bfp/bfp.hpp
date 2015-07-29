/**
 * @file bfp.hpp
 * @author Miroslav Cibulka
 * @brief This is module in which is described Binary File Descriptor as class
 *        and his structures with operations.
 *        @code
 *          auto _bfd = ::BFP::BFD::get_unique_instance();
 *          auto _file = _bfd.open("a.out", "elf64-x86-64");
 *          for (auto _section : _file->sections())
 *            for (auto _symbol : _section->symbols())
 *              process_symbol_in_section(
 *                _section, _symbol);
 *          delete _bfd;
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


/** This macro checks if __type is (const_)iterator of vector of __value */
#define is_iterator(__type, __value)                              \
do {                                                              \
  if (!::std::is_same<                                            \
          __type,                                                 \
          typename ::std::vector<__value>::iterator>::value &&    \
      !::std::is_same<                                            \
          __type,                                                 \
          typename ::std::vector<__value>::const_iterator>::value)\
      RAISE(Exception::BFD::IteratorExpected);                    \
} while (0)


namespace BFP
  {
      template<
          typename __T>
        __T &dereference(__T &_obj)
          {
            return _obj;
          }

      template<
          typename __T>
        const __T &dereference(const __T &_obj)
          {
            return _obj;
          }

      /** Derefer pointer to object 'till only object remain
       * @param __ptr pointer type
       * @param _obj object/pointer to object
       * @return reference to object
       */
      template<
          typename __ptr>
        __ptr &dereference(__ptr *_obj)
          {
            return dereference(*_obj);
          }


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
            __finder_type _val)
          {
            is_iterator(__ite, __value);
            ::std::vector<__value> _ret;
            for (__ite _ite = _begin; _ite != _end; ++_ite)
              if (dereference(*_ite) == _val)
                _ret.push_back(*_ite);
            return _ret;
          }

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
            __finder_type _val)
          {
            is_iterator(__ite, __value);
            __ite _ite;
            for (_ite = _begin; _ite != _end; ++_ite)
              if (dereference(*_ite) == _val) break;
            return _ite;
          }


      namespace __apply
        {
            template<typename T>
              struct function_traits :
                  public function_traits<decltype(&T::operator())>
                {
                };

            /**
             * @brief generates info about function statically
             * @param ClassType is type of class in (lambda) function - deduced
             * @param ReturnType is type of return value in (lambda) function - deduced
             * @param Args are types of arguments in (lambda) function - deduced
             */
            template<
                typename ClassType,
                typename ReturnType,
                typename... Args>
              struct function_traits<ReturnType(ClassType::*)(Args...) const>
                {
                  enum
                    {
                      argc = sizeof...(Args)                   //!< @brief count of arguments
                    };

                  /** @return type of function */
                  typedef ReturnType result_type;

                  /**
                   * @brief set of types in function arguments
                   */
                  template<size_t i>
                    struct argv
                      {
                        /**
                         * @brief the i-th argument is equivalent to the i-th tuple element of a tuple
                         *        composed of those arguments.
                         */
                        typedef typename std::tuple_element<
                            i,
                            std::tuple<Args...>>::type type;
                      };
                };

            /**
             * @brief call function with certain, non-void, return value
             * @param __ret is type of return value
             */
            template<
                typename __ret>
              struct call
                {
                  /** @see ::BFP::apply */
                  template<
                      typename __ite,
                      typename __func,
                      typename __value,
                      typename... __args>
                    static __ret apply(
                        __ite begin,
                        __ite end,
                        __func func,
                        __args... _args)
                      {
                        is_iterator(__ite, __value);
                        auto ret = func(*begin, _args...);
                        for (auto _ite = begin + 1; _ite != end; _ite++)
                          ret = ret + func(*_ite, _args...);
                        return ret;
                      }
                };

            /**
             * @brief call function with void return value
             * @overload
             */
            template<>
              struct call<void>
                {
                  /** @see ::BFP::apply */
                  template<
                      typename __ite,
                      typename __func,
                      typename __value,
                      typename... __args>
                    static void apply(
                        __ite begin,
                        __ite end,
                        __func func,
                        __args... _args)
                      {
                        is_iterator(__ite, __value);
                        for (auto _ite = begin; _ite != end; _ite++)
                          func(*_ite, _args...);
                      }
                };
        }

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
            __args... _args)
          {
            is_iterator(__ite, __value);
            return __apply::call<__ret>::template apply<
                __ite,
                __func,
                __value,
                __args...>(begin, end, func, _args...);
          }

      namespace __filter
        {
            /** @overload */
            template<
                typename __arg,
                typename __func>
              bool helper(
                  __arg _arg,
                  __func _func)
                {
                  return _func(_arg);
                }

            /**
             * @brief Helper template function that calls all function
             *        and returns ...
             */
            template<
                typename __arg,
                typename __func,
                typename... __funcs>
              bool helper(
                  __arg _arg,
                  __func _func,
                  __funcs... _funcs)
                {
                  return _func(_arg) && helper(_arg, _funcs...);
                }
        }

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
            __funcs... _func)
          {
            ::std::vector<typename __ite::value_type> ret;
            for (auto _ite = begin; _ite != end; ++_ite)
              if (__filter::helper(*_ite, _func...))
                ret.push_back(*_ite);
            return ret;
          }

      class Section;

      class Symbol;

      class Symbol
        {
      public:
          ~Symbol()
            { }

          /////////////////////////////////////////////
          ///         Comparition operators         ///
          /////////////////////////////////////////////

          bool operator==(
              const Symbol &_compare)
            {
              return (_compare.getValue() == getValue());
            }

          bool operator!=(
              const Symbol &_compare)
            {
              return (_compare.getValue() != getValue());
            }

          bool operator==(
              const symvalue *_compare)
            {
              return (*_compare == getValue());
            }

          bool operator!=(
              const symvalue *_compare)
            {
              return (*_compare != getValue());
            }

          bool operator==(
              const asymbol *_ptr)
            {
              return _sym == _ptr;
            }

          bool operator!=(
              const asymbol *_ptr)
            {
              return _sym != _ptr;
            }

          bool operator==(
              const char *_compare)
            {
              return (::std::string(_compare) == getName());
            }

          bool operator!=(
              const char *_compare)
            {
              return (::std::string(_compare) != getName());
            }

          ///////////////////////////////
          ///         GETTERS         ///
          ///////////////////////////////

          const ::std::string getName() const noexcept
            {
              return ::std::string(_sym->name);
            }

          /** @return all sections where this symbol is (RO) - may be empty*/
          const ::std::vector<Section *> sections()
            {
              return _sections;
            }

          /**
           * @return begin iterator of sections (RO)
           * @see sections()
           */
          ::std::vector<Section *>::iterator begin_sections()
            {
              return _sections.begin();
            }

          /**
           * @return end iterator of sections (RO)
           * @see sections()
           */
          ::std::vector<Section *>::iterator end_sections()
            {
              return _sections.end();
            }

          /** @return RAW value of symbol */
          symvalue getValue() const noexcept
            {
              return _sym->value;
            }

          /////////////////////////////////////////////////
          ///             Symbol attributes             ///
          /////////////////////////////////////////////////

          bool hasFlags() const noexcept
            {
              return static_cast<bool>(_sym->flags == BSF_NO_FLAGS);
            }

          bool isLocal() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_LOCAL);
            }

          bool isGlobal() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_GLOBAL);
            }

          bool isExported() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_EXPORT);
            }

          bool isFunction() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_FUNCTION);
            }

          bool isDebugging() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_DEBUGGING);
            }

          bool isWeak() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_WEAK);
            }

          bool isSectionSymbol() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_SECTION_SYM);
            }

          bool isOldCommon() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_OLD_COMMON);
            }

          bool isNotAtEnd() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_NOT_AT_END);
            }

          bool isInConstructSection() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_CONSTRUCTOR);
            }

          bool isWarning() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_WARNING);
            }

          bool isIndirect() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_INDIRECT);
            }

          bool hasFileName() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_FILE);
            }

          bool isFromDLI() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_DYNAMIC);
            }

          bool hasObjectData() const noexcept
            {
              return static_cast<bool>(_sym->flags & BSF_OBJECT);
            }

      private:
          /// Only File (factory method) may instantiate Symbol class
          friend class File;

          /**
           *
           */
          Symbol(
              asymbol *symbol)
              :
              _sym{symbol}
            { }

      private:
          /** Appropriate Section to this symbol */
          ::std::vector<Section *> _sections;

          /** BFD symbol structure */
          asymbol *_sym;
        };

      class Section
        {
      public:
          ~Section()
            { }

          /////////////////////////////////////////////
          ///         Comparition operators         ///
          /////////////////////////////////////////////
          bool operator==(
              const Section &_compare)
            {
              return _compare._sec == _sec;
            }

          bool operator!=(
              const Section &_compare)
            {
              return _compare._sec != _sec;
            }

          bool operator==(
              const asection *_ptr)
            {
              return _sec == _ptr;
            }

          bool operator!=(
              const asection *_ptr)
            {
              return _sec != _ptr;
            }

          bool operator==(
              const char *_compare)
            {
              return ::std::string(_compare) == this->getName();
            }

          bool operator!=(
              const char *_compare)
            {
              return ::std::string(_compare) != this->getName();
            }

          ///////////////////////////////
          ///         GETTERS         ///
          ///////////////////////////////

          /** What the section number is in the target world */
          int getIndex() const noexcept
            {
              return _sec->index;
            }

          /**
           * @return name of section like .text, .data or .bss
           */
          const ::std::string getName() const noexcept
            {
              return ::std::string(_sec->name);
            }

          /**
           * @returns content in raw format or nullptr if doesn't have any
           */
          const unsigned char *getContent() const noexcept
            {
              return this->hasContent() ? _sec->contents : nullptr;
            }

          /**
           * @return numbers of line
           *         TODO: findout for what
           */
          const ::std::vector<alent> getLineNO() const noexcept
            {
              return line_numbers;
            }

          /**
           * @return begin iterator of symbols inside this section
           */
          ::std::vector<Symbol *>::iterator begin_symbol()
            {
              return this->_symbols
                         .begin();
            }

          /**
           * @return end iterator of symbols inside this section
           */
          ::std::vector<Symbol *>::iterator end_symbol()
            {
              return this->_symbols
                         .end();
            }

          /**
           * @return vector of appropriate symbols
           */
          const ::std::vector<Symbol *> symbols()
            {
              return this->_symbols;
            }

          //////////////////////////////////////////////////
          ///             Section attributes             ///
          //////////////////////////////////////////////////

          bool hasFlags() const noexcept
            {
              return static_cast<bool>(_sec->flags == SEC_NO_FLAGS);
            }

          bool isAllocOnLoad() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_ALLOC);
            }

          bool isLoadedWithFile() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_LOAD);
            }

          bool hasRellocInfo() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_RELOC);
            }

          bool isReadOnly() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_READONLY);
            }

          bool hasCodeOnly() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_CODE);
            }

          bool hasDataOnly() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_DATA);
            }

          bool isInROM() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_ROM);
            }

          bool hasConstructorInfo() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_CONSTRUCTOR);
            }

          bool hasContent() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_HAS_CONTENTS);
            }

          bool isSuppressed() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_NEVER_LOAD);
            }

          bool isCOFF() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_COFF_SHARED_LIBRARY);
            }

          bool hasCommonSymbols() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_IS_COMMON);
            }

          bool isDebugOnly() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_DEBUGGING);
            }

          bool isInMemory() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_IN_MEMORY);
            }

          bool isExcluded() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_EXCLUDE);
            }

          bool isSorted() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_SORT_ENTRIES);
            }

          bool linkOnce() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_LINK_ONCE);
            }

          bool linkDuplicates() const noexcept
            {
              return static_cast<bool>((_sec->flags & SEC_LINK_DUPLICATES) ==
                                       SEC_LINK_DUPLICATES);
            }

          bool discardDuplicates() const noexcept
            {
              return static_cast<bool>((_sec->flags & SEC_LINK_DUPLICATES) ==
                                       SEC_LINK_DUPLICATES_DISCARD);
            }

          bool linkOneDuplicate() const noexcept
            {
              return static_cast<bool>((_sec->flags & SEC_LINK_DUPLICATES) ==
                                       SEC_LINK_DUPLICATES_ONE_ONLY);
            }

          bool linkSameSizedDuplicates() const noexcept
            {
              return static_cast<bool>((_sec->flags & SEC_LINK_DUPLICATES) ==
                                       SEC_LINK_DUPLICATES_SAME_SIZE);
            }

          bool linkSameDuplicates() const noexcept
            {
              return static_cast<bool>((_sec->flags & SEC_LINK_DUPLICATES) ==
                                       SEC_LINK_DUPLICATES_SAME_CONTENTS);
            }

          bool isCreatedByLinker() const noexcept
            {
              return static_cast<bool>(_sec->flags & SEC_LINKER_CREATED);
            }

      private:
          /// Only File may instantiate this
          friend class File;

          /** This cannot be instantiated outside this class
           *    but it is done via File (factory method)
           * @param section is BFD structure that represents
           *          section of binary file ie. .text, .bss, .data
           */
          Section(
              asection *section)
              :
              _sec{section}
            { }

          /** Cannot be instantiated by primitive constructor */
          Section() = delete;

      private:
          /** Section as BFD structure */
          asection *_sec;

          /** Vector of appropriate symbols */
          ::std::vector<Symbol *> _symbols;

          /** Vector of line numbers - TODO: findout for what is this */
          ::std::vector<alent> line_numbers;
        };

      /** Binary file descriptor class
       * @brief has iterators and is instantiated via BFD singleton/factory
       */
      class File
        {
      public:
          /** @return path to this file */
          const char *get_path() const
            {
              return _path;
            }

          /** @return with which target is this file opened @see BFD::targets */
          const char *get_target() const
            {
              return _target;
            }

          /** @return Used to iterate through sections or finding specific one by name (RO) */
          ::std::vector<Section *>::iterator begin_section()
            {
              return this->_sections
                         .begin();
            }

          /** @return Used to iterate through sections or finding specific one by name (RO) */
          ::std::vector<Section *>::iterator end_section()
            {
              return this->_sections
                         .end();
            }

          /** @return Iterator through all symbols in file (RO) */
          ::std::vector<Symbol *>::iterator begin_symbol()
            {
              return this->_symbols
                         .begin();
            }

          /** @return Iterator through all symbols in file (RO) */
          ::std::vector<Symbol *>::iterator end_symbol()
            {
              return this->_symbols
                         .end();
            }

          /** @return vector of all symbols (RO) */
          const ::std::vector<Symbol *> symbols()
            {
              return this->_symbols;
            }

          /** @return vector of all sections (RO) */
          const ::std::vector<Section *> sections()
            {
              return this->_sections;
            }


          /** Frees all sections and symbols when deleted
           *    By default it is done by BFD so ... don't do it
           */
          ~File();

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

      private:
          /** File descriptor */
          bfd *_fd;

          /** Path to executable */
          const char *_path;

          /** Target name eg. elf64-x86-64 */
          const char *_target;

          /** Vector of section in file (contains also appropriate symbols) */
          ::std::vector<Section *> _sections;

          /** Vector of symbols in file */
          ::std::vector<Symbol *> _symbols;

          /** BFD Symbol table */
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

          /** @return all possible formats */
          ::std::vector<::std::string> getAllTargets() const noexcept
            {
              return _targets;
            }

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

      private:
          ::std::vector<::std::string> _targets = {
              "a.out-i386-linux", "elf32-i386", "elf64-big", "elf64-little",
              "pei-i386", "srec", "verilog", "binary", "elf32-little",
              "elf64-k1om", "elf64-x86-64", "pei-x86-64", "symbolsrec",
              "elf32-big", "elf32-x86-64", "elf64-l1om", "ihex", "plugin",
              "tekhex"
          };
        };
  }

#endif // __BFP_BFP_HPP
