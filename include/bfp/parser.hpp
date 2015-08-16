/**
 * @file parser.hpp
 * @author Miroslav Cibulka
 * @brief This is module in which is described Binary File Descriptor as class
 *        and his structures with operations.
 *        TODO: detailed description
 */

#ifndef __BFP_BFP_HPP
# define __BFP_BFP_HPP

#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif


namespace bfp
  {
      /** Singleton of Binary File Descriptor
       * @brief This represents BFD class.
       *        Instance is static so there will be only one per runtime.
       *        <tt>get_unique_instance</tt> returns the same instance every time it is called.
       * @example tests/SymbolReader/main.cpp
       */
      class Parser
        {
      public:
          /** Get unique instance of BFD
           *    (may be called multiple times but same object is returned)
           * @return static instance of BFD
           * @see BFD
           */
          static Parser *get_unique_instance();

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
          ~Parser();

      private:
          /** It is forbidden to create instance anywhere but in this class only
           *    (once at the beginning)
           */
          Parser();

          /** Forbidden copy constructor */
          Parser(const Parser &) = delete;

          /** Forbidden move constructor */
          Parser(Parser &&) = delete;

          /** Forbidden copy assignment */
          Parser &operator=(const Parser &) = delete;

          /** Forbidden move assignment */
          Parser &operator=(Parser &&) = delete;

      public:
          /** This is public so everyone may use this
           *      Feel free to deallocate whatever file when needed but has to be
           *      erased from this vector afterwards (SIGSEGV - double free/delete)
           */
          ::std::vector<File *> openedFiles;

      private:
          /** vector of all possible targets */
          ::std::vector<::std::string> _targets;
        };
  }


#endif // __BFP_BFP_HPP
