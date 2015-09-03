/**
 * @file exception.cpp
 * @author Miroslav Cibulka
 * @brief Exception system for Binary File Parser
 */

#include "../include/bfp.hpp"


#define EX_HEADER(msg, call, ex_msg) {                 \
    msg += "Last called function:\n\t" + call          \
        +  ::std::string("\n\nBrief:\n\t") + ex_msg;   \
}

::std::string bfp::base_exception::get_backtrace()
  {
    void *buffer[200];
    int n;
    char **strings;
    ::std::string msg;

    n = backtrace(buffer, 200);
    strings = backtrace_symbols(buffer, n);
    if (strings == NULL)
      {
        msg += "Error on backtracking\n";
        return msg;
      }
    else
      {
        msg += ::std::string("\nFunctions backtrace:\n");
      }

    for (int j = n - 1;
         j >= 3;
         j--)
      {
        msg += ::std::string("\t") + std::to_string(n - j) + ": " +
               ::std::string(strings[j]) + "\n";
      }
    free(strings);
    return msg;
  }

::bfp::Exception::Parser::Parser(
    const char *ex_msg,
    ::std::string LastCall)
  {
    EX_HEADER(msg, LastCall, ex_msg);
    msg += "\n" + get_backtrace();
  }

::bfp::Exception::Parser::NoError::NoError(::std::string LastCall)
    :
    Parser("Internal Error", LastCall)
  { }

::bfp::Exception::Parser::SystemCall::SystemCall(::std::string LastCall)
    :
    Parser(strerror(errno), LastCall)
  { }

::bfp::Exception::Parser::InvalidTarget::InvalidTarget(::std::string LastCall)
    :
    Parser("Invalid target", LastCall)
  { }

::bfp::Exception::Parser::WrongFormat::WrongFormat(::std::string LastCall)
    :
    Parser("Wrong format", LastCall)
  { }

::bfp::Exception::Parser::InvalidOperation::InvalidOperation(::std::string LastCall)
    :
    Parser("Invalid operation", LastCall)
  { }

::bfp::Exception::Parser::NoMemory::NoMemory(::std::string LastCall)
    :
    Parser("NoMemory", LastCall)
  { }

::bfp::Exception::Parser::NoSymbols::NoSymbols(::std::string LastCall)
    :
    Parser("NoSymbols", LastCall)
  { }

::bfp::Exception::Parser::NoArmap::NoArmap(::std::string LastCall)
    :
    Parser("NoArmap", LastCall)
  { }

::bfp::Exception::Parser::NoMoreArchivedFiles::NoMoreArchivedFiles(::std::string LastCall)
    :
    Parser("NoMoreArchivedFiles", LastCall)
  { }

::bfp::Exception::Parser::MalformedArchive::MalformedArchive(::std::string LastCall)
    :
    Parser("MalformedArchive", LastCall)
  { }

::bfp::Exception::Parser::FileNotRecognized::FileNotRecognized(::std::string LastCall)
    :
    Parser("FileNotRecognized", LastCall)
  { }

::bfp::Exception::Parser::FileAmbiguouslyRecognized::FileAmbiguouslyRecognized(::std::string LastCall)
    :
    Parser("FileAmbiguouslyRecognized", LastCall)
  { }

::bfp::Exception::Parser::NoContents::NoContents(::std::string LastCall)
    :
    Parser("NoContents", LastCall)
  { }

::bfp::Exception::Parser::NonrepresentableSection::NonrepresentableSection(::std::string LastCall)
    :
    Parser("NonrepresentableSection", LastCall)
  { }

::bfp::Exception::Parser::NoDebugSection::NoDebugSection(::std::string LastCall)
    :
    Parser("NoDebugSection", LastCall)
  { }

::bfp::Exception::Parser::BadValue::BadValue(::std::string LastCall)
    :
    Parser("BadValue", LastCall)
  { }

::bfp::Exception::Parser::FileTruncated::FileTruncated(::std::string LastCall)
    :
    Parser("FileTruncated", LastCall)
  { }

::bfp::Exception::Parser::FileTooBig::FileTooBig(::std::string LastCall)
    :
    Parser("FileTooBig", LastCall)
  { }

::bfp::Exception::Parser::InvalidErrorCode::InvalidErrorCode(::std::string LastCall)
    :
    Parser("InvalidErrorCode", LastCall)
  { }

::bfp::Exception::Parser::WrongObjectFormat::WrongObjectFormat(::std::string LastCall)
    :
    Parser("WrongObjectFormat", LastCall)
  { }

::bfp::Exception::Parser::MissingDso::MissingDso(::std::string LastCall)
    :
    Parser("MissingDso", LastCall)
  { }

::bfp::Exception::Parser::OnInput::OnInput(::std::string LastCall)
    :
    Parser("OnInput", LastCall)
  { }

::bfp::Exception::Parser::IteratorExpected::IteratorExpected(::std::string LastCall)
    :
    Parser("IteratorExpected", LastCall)
  { }

::bfp::Exception::Plugins::Plugins(
    const char *ex_msg,
    ::std::string LastCall)
  {
    EX_HEADER(msg, LastCall, ex_msg);
    msg += "\n" + get_backtrace();
  }

::bfp::Exception::Plugins::FileIsNotADir::FileIsNotADir(::std::string LastCall)
    :
    Plugins(
        "Path provided to Plugin Manager is not a directory (should be directory with plugins)",
        LastCall)
  { }

::bfp::Exception::Plugins::StageDirExpected::StageDirExpected(::std::string LastCall)
    :
    Plugins("In plugin directory are stage directories expected", LastCall)
  { }

::bfp::Exception::Plugins::DLError::DLError(::std::string LastCall)
    :
    Plugins("Plugin's array was not found in plugin!", LastCall)
  { }
