/**
 * @file exception.hpp
 * @author Miroslav Cibulka
 * @brief Exception system for Binary File Parser
 */

#ifndef BINARY_FILE_PARSER_EXCEPTION_HPP
#define BINARY_FILE_PARSER_EXCEPTION_HPP

#include <iostream>
#include <execinfo.h>
#include <bfd.h>
#include <dlfcn.h>
#include <cstring>

#define RAISE(ex) throw ex(std::string(__FILE__) + ":" + ::std::to_string(__LINE__) + ":" + __FUNCTION__ + "()")

namespace BFP {

    /** Exceptions system for BFD
     * @brief This is virtual class for BFD exceptions with function backtrack
     */
    class Exception : ::std::exception {
    public:

        /** May be thrown on closing file */
        class Closing;

        /** May be thrown on opening file */
        class Opening;

        /** May be thrown on loading plugin */
        class LoadingPlugin;

        /** May be thrown on loading plugin file */
        class PluginsArrNotExists;

        /** May be thrown on creating instance of plugin class */
        class CreatingPluginInstance;

        /** May be thrown on creating instance of plugin class */
        class PluginsArrNotExists;

        /** Constructs Exception with backtrace, last called method and message
         * @brief Translates various errors to strings
         */
        Exception(const char *_msg, ::std::string LastCall = "Unknown") {
            void *buffer[200];
            int n;
            char **strings;
            msg += "Last called function:\n\t" + LastCall;
            msg += ::std::string("\n\nBrief:\n\t") + _msg;
            msg += "\n\nDescription:\n\t";
            switch (bfd_get_error()) {
                case bfd_error_type::bfd_error_no_error:
                    char *error;
                    if (errno)
                        msg += ::std::string("System error: ") + strerror(errno);
                    else if ((error = dlerror()) != NULL)
                        msg += ::std::string("Dynamic linking error: ") + error;
                    else
                        msg += "Not a BFD/System/DL error";
                    break;
                case bfd_error_type::bfd_error_system_call:
                    msg += ::std::string("System call: ") + strerror(errno);
                    break;
                case bfd_error_type::bfd_error_invalid_target:
                    msg += "Invalid target";
                    break;
                case bfd_error_type::bfd_error_wrong_format:
                    msg += "Wrong format";
                    break;
                case bfd_error_type::bfd_error_invalid_operation:
                    msg += "Invalid operation";
                    break;
                case bfd_error_type::bfd_error_no_memory:
                    msg += "No memory";
                    break;
                case bfd_error_type::bfd_error_no_symbols:
                    msg += "No symbols";
                    break;
                case bfd_error_type::bfd_error_no_armap:
                    msg += "No armap";
                    break;
                case bfd_error_type::bfd_error_no_more_archived_files:
                    msg += "No more archived files";
                    break;
                case bfd_error_type::bfd_error_malformed_archive:
                    msg += "Malformed archive";
                    break;
                case bfd_error_type::bfd_error_file_not_recognized:
                    msg += "File not recognized";
                    break;
                case bfd_error_type::bfd_error_file_ambiguously_recognized:
                    msg += "File ambiguously recognized";
                    break;
                case bfd_error_type::bfd_error_no_contents:
                    msg += "No contents";
                    break;
                case bfd_error_type::bfd_error_nonrepresentable_section:
                    msg += "Nonrepresentable section";
                    break;
                case bfd_error_type::bfd_error_no_debug_section:
                    msg += "No debug section";
                    break;
                case bfd_error_type::bfd_error_bad_value:
                    msg += "Bad value";
                    break;
                case bfd_error_type::bfd_error_file_truncated:
                    msg += "File truncated";
                    break;
                case bfd_error_type::bfd_error_file_too_big:
                    msg += "File is too big";
                    break;
                case bfd_error_type::bfd_error_invalid_error_code:
                    msg += "Invalid error code";
                    break;
                case bfd_error_type::bfd_error_wrong_object_format:
                    msg += "Wrong object format";
                    break;
                case bfd_error_type::bfd_error_missing_dso:
                    msg += "Missing DSO";
                    break;
                case bfd_error_type::bfd_error_on_input:
                    msg += "Input error";
                    break;
            }
            msg += "\n";
            n = backtrace(buffer, 200);
            strings = backtrace_symbols(buffer, n);
            if (strings == NULL) {
                msg += "Error on backtracking\n";
                return;
            } else {
                msg += ::std::string("\nFunctions backtrace:\n");
            }

            for (int j = n - 1; j >= 0; j--)
                msg += ::std::string("\t") + std::to_string(n - j) + ": " +
                       ::std::string(strings[j]) + "\n";

            free(strings);
        }

        /** Returns message about error */
        virtual const char *what() const throw() {
            return msg.c_str();
        }

    private:
        ::std::string msg;
    };

    class Exception::Closing : public Exception {
    public:
        Closing(::std::string LastCall) :
                Exception("Exception occurred on closing file descriptor",
                          LastCall) { }
    };

    class Exception::Opening : public Exception {
    public:
        Opening(::std::string LastCall) :
                Exception("Exception occurred on opening file descriptor",
                          LastCall) { }
    };

    class Exception::LoadingPlugin : public Exception {
    public:
        LoadingPlugin(::std::string LastCall) :
                Exception("Exception occurred on opening dynamically linked plugin",
                          LastCall) { }
    };


    class Exception::CreatingPluginInstance : public Exception {
    public:
        CreatingPluginInstance(::std::string LastCall) :
                Exception("Exception occurred on creating instance of plugin class",
                          LastCall) { }
    };

    class Exception::PluginsArrNotExists : public Exception {
    public:
        PluginsArrNotExists(::std::string LastCall) :
                Exception("Plugin's array was not found in plugin!",
                          LastCall) { }
    };
}
#endif //BINARY_FILE_PARSER_EXCEPTION_HPP
