/**
 * @file exception.hpp
 * @author Miroslav Cibulka
 * @brief Exception system for Binary File Parser
 */

#ifndef BINARY_FILE_PARSER_EXCEPTION_HPP
#define BINARY_FILE_PARSER_EXCEPTION_HPP

#include <iostream>
#include <execinfo.h>
#include <dlfcn.h>
#include <cstring>
#include <bfd.h>

/** Use THIS instead of throw */
#define RAISE(ex) throw ex(std::string(__FILE__) + ":" + ::std::to_string(__LINE__) + ":" + __FUNCTION__ + "()")

/** Asserts known errors and throws them */
#define BFP_ASSERT() do {                                                   \
    char *err;                                                              \
    if ((err = dlerror()) != NULL)                                          \
        RAISE(::BFP::Exception::Plugins::DLError);                          \
    switch (bfd_get_error()) {                                              \
        case bfd_error_type::bfd_error_no_error:                            \
            RAISE(::BFP::Exception::BFD::NoError);                          \
        case bfd_error_type::bfd_error_system_call:                         \
            RAISE(::BFP::Exception::BFD::SystemCall);                       \
        case bfd_error_type::bfd_error_invalid_target:                      \
            RAISE(::BFP::Exception::BFD::InvalidTarget);                    \
        case bfd_error_type::bfd_error_wrong_format:                        \
            RAISE(::BFP::Exception::BFD::WrongFormat);                      \
        case bfd_error_type::bfd_error_invalid_operation:                   \
            RAISE(::BFP::Exception::BFD::InvalidOperation);                 \
        case bfd_error_type::bfd_error_no_memory:                           \
            RAISE(::BFP::Exception::BFD::NoMemory);                         \
        case bfd_error_type::bfd_error_no_symbols:                          \
            RAISE(::BFP::Exception::BFD::NoSymbols);                        \
        case bfd_error_type::bfd_error_no_armap:                            \
            RAISE(::BFP::Exception::BFD::NoArmap);                          \
        case bfd_error_type::bfd_error_no_more_archived_files:              \
            RAISE(::BFP::Exception::BFD::NoMoreArchivedFiles);              \
        case bfd_error_type::bfd_error_malformed_archive:                   \
            RAISE(::BFP::Exception::BFD::MalformedArchive);                 \
        case bfd_error_type::bfd_error_file_not_recognized:                 \
            RAISE(::BFP::Exception::BFD::FileNotRecognized);                \
        case bfd_error_type::bfd_error_file_ambiguously_recognized:         \
            RAISE(::BFP::Exception::BFD::FileAmbiguouslyRecognized);        \
        case bfd_error_type::bfd_error_no_contents:                         \
            RAISE(::BFP::Exception::BFD::NoContents);                       \
        case bfd_error_type::bfd_error_nonrepresentable_section:            \
            RAISE(::BFP::Exception::BFD::NonrepresentableSection);          \
        case bfd_error_type::bfd_error_no_debug_section:                    \
            RAISE(::BFP::Exception::BFD::NoDebugSection);                   \
        case bfd_error_type::bfd_error_bad_value:                           \
            RAISE(::BFP::Exception::BFD::BadValue);                         \
        case bfd_error_type::bfd_error_file_truncated:                      \
            RAISE(::BFP::Exception::BFD::FileTruncated);                    \
        case bfd_error_type::bfd_error_file_too_big:                        \
            RAISE(::BFP::Exception::BFD::FileTooBig);                       \
        case bfd_error_type::bfd_error_invalid_error_code:                  \
            RAISE(::BFP::Exception::BFD::InvalidErrorCode);                 \
        case bfd_error_type::bfd_error_wrong_object_format:                 \
            RAISE(::BFP::Exception::BFD::WrongObjectFormat);                \
        case bfd_error_type::bfd_error_on_input:                            \
            RAISE(::BFP::Exception::BFD::OnInput);                          \
        default:                                                            \
            break;                                                          \
    }                                                                       \
} while(0)

namespace BFP {

    /** ::BFP::base_exception
     * @brief Base class for all BFP exceptions (for exception isolation from system or similar)
     *        Use this to catch all BFP exceptions!
     */
    class base_exception : public ::std::exception {
    protected:
        /** Returns function backtrace */
        ::std::string get_backtrace();
    };

    namespace Exception {
        /** Exceptions system for BFD
         * @brief Basel class for BFD exceptions with function backtrack
         */
        class BFD;

        /** exception system for Plugins
         * @brief Base class for plugin exceptions with function backtrack
         */
        class Plugins;


        class BFD : public ::BFP::base_exception {
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

            /**
             * @param ex_msg is message from thrower
             * @param LastCall is name of last called method inserted by macro RAISE
             */
            BFD(const char *ex_msg, ::std::string LastCall = "Unknown");

            const virtual char *what() const throw() {
                return msg.c_str();
            }

        private:
            ::std::string msg;
        };

        class Plugins  : public ::BFP::base_exception {
        public:
            class DLError;
            class FileIsNotADir;
            class StageDirExpected;

            /**
             * @param ex_msg is message from thrower
             * @param LastCall is name of last called method inserted by macro RAISE
             */
            Plugins(const char *ex_msg, ::std::string LastCall = "Unknown");

            const char virtual *what() const throw() {
                return msg.c_str();
            }

        private:
            ::std::string msg;
        };

        class BFD::NoError : public BFD {
        public:
            NoError(::std::string LastCall);
        };

        class BFD::SystemCall : public BFD {
        public:
            SystemCall(::std::string LastCall);
        };

        class BFD::InvalidTarget : public BFD {
        public:
            InvalidTarget(::std::string LastCall);
        };

        class BFD::WrongFormat : public BFD {
        public:
            WrongFormat(::std::string LastCall);
        };

        class BFD::InvalidOperation : public BFD {
        public:
            InvalidOperation(::std::string LastCall);
        };

        class BFD::NoMemory : public BFD {
        public:
            NoMemory(::std::string LastCall);
        };

        class BFD::NoSymbols : public BFD {
        public:
            NoSymbols(::std::string LastCall);
        };

        class BFD::NoArmap : public BFD {
        public:
            NoArmap(::std::string LastCall);
        };

        class BFD::NoMoreArchivedFiles : public BFD {
        public:
            NoMoreArchivedFiles(::std::string LastCall);
        };

        class BFD::MalformedArchive : public BFD {
        public:
            MalformedArchive(::std::string LastCall);
        };

        class BFD::FileNotRecognized : public BFD {
        public:
            FileNotRecognized(::std::string LastCall);
        };

        class BFD::FileAmbiguouslyRecognized : public BFD {
        public:
            FileAmbiguouslyRecognized(::std::string LastCall);
        };

        class BFD::NoContents : public BFD {
        public:
            NoContents(::std::string LastCall);
        };

        class BFD::NonrepresentableSection : public BFD {
        public:
            NonrepresentableSection(::std::string LastCall);
        };

        class BFD::NoDebugSection : public BFD {
        public:
            NoDebugSection(::std::string LastCall);
        };

        class BFD::BadValue : public BFD {
        public:
            BadValue(::std::string LastCall);
        };

        class BFD::FileTruncated : public BFD {
        public:
            FileTruncated(::std::string LastCall);
        };

        class BFD::FileTooBig : public BFD {
        public:
            FileTooBig(::std::string LastCall);
        };

        class BFD::InvalidErrorCode : public BFD {
        public:
            InvalidErrorCode(::std::string LastCall);
        };

        class BFD::WrongObjectFormat : public BFD {
        public:
            WrongObjectFormat(::std::string LastCall);
        };

        class BFD::MissingDso : public BFD {
        public:
            MissingDso(::std::string LastCall);
        };

        class BFD::OnInput : public BFD {
        public:
            OnInput(::std::string LastCall);
        };

        class Plugins::DLError : public Plugins {
        public:
            DLError(::std::string LastCall);
        };

        class Plugins::FileIsNotADir : public Plugins {
        public:
            FileIsNotADir(::std::string LastCall);
        };

        class Plugins::StageDirExpected : public Plugins {
        public:
            StageDirExpected(::std::string LastCall);
        };
    }
}
#endif //BINARY_FILE_PARSER_EXCEPTION_HPP
