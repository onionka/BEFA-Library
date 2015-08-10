/**
 * @file exception.hpp
 * @author Miroslav Cibulka
 * @brief Exception system for Binary File Parser
 */

#ifndef __BFP_EXCEPTION_HPP
# define __BFP_EXCEPTION_HPP

#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif

#include <iostream>
#include <execinfo.h>
#include <dlfcn.h>
#include <cstring>
#include <bfd.h>


/** Use THIS instead of throw */
#define RAISE(ex)\
  throw ex(std::string(__FILE__) + ":" + ::std::to_string(__LINE__) + ":" + __FUNCTION__ + "()")

/** Asserts known errors and throws them */
#define BFP_ASSERT() do {                                                   \
    char *err;                                                              \
    if ((err = dlerror()) != NULL)                                          \
        RAISE(::bfp::Exception::Plugins::DLError);                          \
    switch (bfd_get_error()) {                                              \
        case bfd_error_type::bfd_error_no_error:                            \
            RAISE(::bfp::Exception::Parser::NoError);                          \
        case bfd_error_type::bfd_error_system_call:                         \
            RAISE(::bfp::Exception::Parser::SystemCall);                       \
        case bfd_error_type::bfd_error_invalid_target:                      \
            RAISE(::bfp::Exception::Parser::InvalidTarget);                    \
        case bfd_error_type::bfd_error_wrong_format:                        \
            RAISE(::bfp::Exception::Parser::WrongFormat);                      \
        case bfd_error_type::bfd_error_invalid_operation:                   \
            RAISE(::bfp::Exception::Parser::InvalidOperation);                 \
        case bfd_error_type::bfd_error_no_memory:                           \
            RAISE(::bfp::Exception::Parser::NoMemory);                         \
        case bfd_error_type::bfd_error_no_symbols:                          \
            RAISE(::bfp::Exception::Parser::NoSymbols);                        \
        case bfd_error_type::bfd_error_no_armap:                            \
            RAISE(::bfp::Exception::Parser::NoArmap);                          \
        case bfd_error_type::bfd_error_no_more_archived_files:              \
            RAISE(::bfp::Exception::Parser::NoMoreArchivedFiles);              \
        case bfd_error_type::bfd_error_malformed_archive:                   \
            RAISE(::bfp::Exception::Parser::MalformedArchive);                 \
        case bfd_error_type::bfd_error_file_not_recognized:                 \
            RAISE(::bfp::Exception::Parser::FileNotRecognized);                \
        case bfd_error_type::bfd_error_file_ambiguously_recognized:         \
            RAISE(::bfp::Exception::Parser::FileAmbiguouslyRecognized);        \
        case bfd_error_type::bfd_error_no_contents:                         \
            RAISE(::bfp::Exception::Parser::NoContents);                       \
        case bfd_error_type::bfd_error_nonrepresentable_section:            \
            RAISE(::bfp::Exception::Parser::NonrepresentableSection);          \
        case bfd_error_type::bfd_error_no_debug_section:                    \
            RAISE(::bfp::Exception::Parser::NoDebugSection);                   \
        case bfd_error_type::bfd_error_bad_value:                           \
            RAISE(::bfp::Exception::Parser::BadValue);                         \
        case bfd_error_type::bfd_error_file_truncated:                      \
            RAISE(::bfp::Exception::Parser::FileTruncated);                    \
        case bfd_error_type::bfd_error_file_too_big:                        \
            RAISE(::bfp::Exception::Parser::FileTooBig);                       \
        case bfd_error_type::bfd_error_invalid_error_code:                  \
            RAISE(::bfp::Exception::Parser::InvalidErrorCode);                 \
        case bfd_error_type::bfd_error_wrong_object_format:                 \
            RAISE(::bfp::Exception::Parser::WrongObjectFormat);                \
        case bfd_error_type::bfd_error_on_input:                            \
            RAISE(::bfp::Exception::Parser::OnInput);                          \
        default:                                                            \
            break;                                                          \
    }                                                                       \
} while(0)

namespace bfp
  {

      /** ::BFP::base_exception
       * @brief Base class for all BFP exceptions (for exception isolation from system or similar)
       *        Use this to catch all BFP exceptions!
       */
      class base_exception :
          public ::std::exception
        {
      protected:
          /** Returns function backtrace */
          ::std::string get_backtrace();
        };

      namespace Exception
        {
            /** Exceptions system for Parser
             * @brief Basel class for Parser exceptions with function backtrack
             */
            class Parser;

            /** exception system for Plugins
             * @brief Base class for plugin exceptions with function backtrack
             */
            class Plugins;


            class Parser :
                public ::bfp::base_exception
              {
            public:
                class NoError;

                class SystemCall;

                class InvalidTarget;

                class WrongFormat;

                class InvalidOperation;

                class NoMemory;

                class NoSymbols;

                class NoArmap;

                class NoMoreArchivedFiles;

                class MalformedArchive;

                class FileNotRecognized;

                class FileAmbiguouslyRecognized;

                class NoContents;

                class NonrepresentableSection;

                class NoDebugSection;

                class BadValue;

                class FileTruncated;

                class FileTooBig;

                class InvalidErrorCode;

                class WrongObjectFormat;

                class MissingDso;

                class OnInput;

                class IteratorExpected;

                /**
                 * @param ex_msg is message from thrower
                 * @param LastCall is name of last called method inserted by macro RAISE
                 */
                Parser(
                    const char *ex_msg,
                    ::std::string LastCall = "Unknown");

                const virtual char *what() const throw()
                  {
                    return msg.c_str();
                  }

            private:
                ::std::string msg;
              };

            class Plugins :
                public ::bfp::base_exception
              {
            public:
                class DLError;

                class FileIsNotADir;

                class StageDirExpected;

                /**
                 * @param ex_msg is message from thrower
                 * @param LastCall is name of last called method inserted by macro RAISE
                 */
                Plugins(
                    const char *ex_msg,
                    ::std::string LastCall = "Unknown");

                const char virtual *what() const throw()
                  {
                    return msg.c_str();
                  }

            private:
                ::std::string msg;
              };

            class Parser::NoError :
                public Parser
              {
            public:
                NoError(::std::string LastCall);
              };

            class Parser::SystemCall :
                public Parser
              {
            public:
                SystemCall(::std::string LastCall);
              };

            class Parser::InvalidTarget :
                public Parser
              {
            public:
                InvalidTarget(::std::string LastCall);
              };

            class Parser::WrongFormat :
                public Parser
              {
            public:
                WrongFormat(::std::string LastCall);
              };

            class Parser::InvalidOperation :
                public Parser
              {
            public:
                InvalidOperation(::std::string LastCall);
              };

            class Parser::NoMemory :
                public Parser
              {
            public:
                NoMemory(::std::string LastCall);
              };

            class Parser::NoSymbols :
                public Parser
              {
            public:
                NoSymbols(::std::string LastCall);
              };

            class Parser::NoArmap :
                public Parser
              {
            public:
                NoArmap(::std::string LastCall);
              };

            class Parser::NoMoreArchivedFiles :
                public Parser
              {
            public:
                NoMoreArchivedFiles(::std::string LastCall);
              };

            class Parser::MalformedArchive :
                public Parser
              {
            public:
                MalformedArchive(::std::string LastCall);
              };

            class Parser::FileNotRecognized :
                public Parser
              {
            public:
                FileNotRecognized(::std::string LastCall);
              };

            class Parser::FileAmbiguouslyRecognized :
                public Parser
              {
            public:
                FileAmbiguouslyRecognized(::std::string LastCall);
              };

            class Parser::NoContents :
                public Parser
              {
            public:
                NoContents(::std::string LastCall);
              };

            class Parser::NonrepresentableSection :
                public Parser
              {
            public:
                NonrepresentableSection(::std::string LastCall);
              };

            class Parser::NoDebugSection :
                public Parser
              {
            public:
                NoDebugSection(::std::string LastCall);
              };

            class Parser::BadValue :
                public Parser
              {
            public:
                BadValue(::std::string LastCall);
              };

            class Parser::FileTruncated :
                public Parser
              {
            public:
                FileTruncated(::std::string LastCall);
              };

            class Parser::FileTooBig :
                public Parser
              {
            public:
                FileTooBig(::std::string LastCall);
              };

            class Parser::InvalidErrorCode :
                public Parser
              {
            public:
                InvalidErrorCode(::std::string LastCall);
              };

            class Parser::WrongObjectFormat :
                public Parser
              {
            public:
                WrongObjectFormat(::std::string LastCall);
              };

            class Parser::MissingDso :
                public Parser
              {
            public:
                MissingDso(::std::string LastCall);
              };

            class Parser::OnInput :
                public Parser
              {
            public:
                OnInput(::std::string LastCall);
              };

            class Parser::IteratorExpected :
                public Parser
              {
            public:
                IteratorExpected(::std::string LastCall);
              };

            class Plugins::DLError :
                public Plugins
              {
            public:
                DLError(::std::string LastCall);
              };

            class Plugins::FileIsNotADir :
                public Plugins
              {
            public:
                FileIsNotADir(::std::string LastCall);
              };

            class Plugins::StageDirExpected :
                public Plugins
              {
            public:
                StageDirExpected(::std::string LastCall);
              };
        }
  }
#endif //__BFP_EXCEPTION_HPP
