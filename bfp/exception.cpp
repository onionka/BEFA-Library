/**
 * @file exception.cpp
 * @author Miroslav Cibulka
 * @brief Exception system for Binary File Parser
 */

#include <bfp/exception.h>

#define EX_HEADER(msg, call, ex_msg) {                 \
    msg += "Last called function:\n\t" + LastCall;      \
    msg += ::std::string("\n\nBrief:\n\t") + ex_msg;   \
    msg += "\n\nDescription:\n\t";                      \
}


::std::string BFP::Exception::get_backtrace() {
    void *buffer[200];
    int n;
    char **strings;
    ::std::string msg;

    n = backtrace(buffer, 200);
    strings = backtrace_symbols(buffer, n);
    if (strings == NULL) {
        msg += "Error on backtracking\n";
        return msg;
    } else {
        msg += ::std::string("\nFunctions backtrace:\n");
    }

    for (int j = n - 1; j >= 3; j--) {
        msg += ::std::string("\t") + std::to_string(n - j) + ": " +
               ::std::string(strings[j]) + "\n";
    }
    free(strings);
    return msg;
}

BFP::Exception::BFD::BFD(const char *ex_msg,
                                 ::std::string LastCall) {
    EX_HEADER(msg, LastCall, ex_msg);
    switch (bfd_get_error()) {
        case bfd_error_type::bfd_error_no_error:
            if (errno)
                msg += ::std::string("System error: ") +
                       strerror(errno);
            else
                msg += "Internal Error";
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
///////////////////////////////////////////////////////////////////////
//           This is missing in Ubuntu 12.4
//              case bfd_error_type::bfd_error_missing_dso:
//                  msg += "Missing DSO";
//                  break;
///////////////////////////////////////////////////////////////////////
        case bfd_error_type::bfd_error_on_input:
            msg += "Input error";
            break;
        default:
            msg += "Undefined BFD error";
            break;
    }
    msg += "\n" + get_backtrace();
}

BFP::Exception::BFD::Closing::Closing(
    ::std::string LastCall)
        : BFD(
            "BFD occurred on closing file descriptor",
            LastCall) { }

BFP::Exception::BFD::Opening::Opening(
    ::std::string LastCall)
        : BFD(
            "BFD occurred on opening file descriptor",
            LastCall) { }

BFP::Exception::Plugins::Plugins(const char *ex_msg,
                                   ::std::string LastCall) {
    EX_HEADER(msg, LastCall, ex_msg);
    char *error;
    if ((error = dlerror()) != NULL)
        msg += ::std::string("Dynamic linking error: ") + error;
    else
        msg += "Not a BFD/System/DL error";
}

BFP::Exception::Plugins::LoadingPlugin::LoadingPlugin(
    ::std::string LastCall)
        : Plugins(
            "BFD occurred on opening dynamically linked plugin",
            LastCall) { }

BFP::Exception::Plugins::CreatingPluginInstance::CreatingPluginInstance(
    ::std::string LastCall)
        : Plugins(
            "BFD occurred on creating instance of plugin class",
            LastCall) { }

BFP::Exception::Plugins::PluginsArrNotExists::PluginsArrNotExists(
    ::std::string LastCall)
        : Plugins(
            "Plugin's array was not found in plugin!",
            LastCall) { }
