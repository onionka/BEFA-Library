/**
 * @file exception.cpp
 * @author Miroslav Cibulka
 * @brief Exception system for Binary File Parser
 */

#include <bfp.hpp>


#define EX_HEADER(msg, call, ex_msg) {                 \
    msg += "Last called function:\n\t" + call          \
        +  ::std::string("\n\nBrief:\n\t") + ex_msg;   \
}

::std::string BFP::base_exception::get_backtrace()
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

    for (int j = n - 1; j >= 3; j--)
      {
        msg += ::std::string("\t") + std::to_string(n - j) + ": " +
               ::std::string(strings[j]) + "\n";
      }
    free(strings);
    return msg;
  }

::BFP::Exception::BFD::BFD(
    const char *ex_msg,
    ::std::string LastCall)
  {
    EX_HEADER(msg, LastCall, ex_msg);
    msg += "\n" + get_backtrace();
  }

::BFP::Exception::BFD::NoError::NoError(::std::string LastCall)
    :
    BFD("Internal Error", LastCall)
  { }

::BFP::Exception::BFD::SystemCall::SystemCall(::std::string LastCall)
    :
    BFD(strerror(errno), LastCall)
  { }

::BFP::Exception::BFD::InvalidTarget::InvalidTarget(::std::string LastCall)
    :
    BFD("Invalid target", LastCall)
  { }

::BFP::Exception::BFD::WrongFormat::WrongFormat(::std::string LastCall)
    :
    BFD("Wrong format", LastCall)
  { }

::BFP::Exception::BFD::InvalidOperation::InvalidOperation(::std::string LastCall)
    :
    BFD("Invalid operation", LastCall)
  { }

::BFP::Exception::BFD::NoMemory::NoMemory(::std::string LastCall)
    :
    BFD("NoMemory", LastCall)
  { }

::BFP::Exception::BFD::NoSymbols::NoSymbols(::std::string LastCall)
    :
    BFD("NoSymbols", LastCall)
  { }

::BFP::Exception::BFD::NoArmap::NoArmap(::std::string LastCall)
    :
    BFD("NoArmap", LastCall)
  { }

::BFP::Exception::BFD::NoMoreArchivedFiles::NoMoreArchivedFiles(::std::string LastCall)
    :
    BFD("NoMoreArchivedFiles", LastCall)
  { }

::BFP::Exception::BFD::MalformedArchive::MalformedArchive(::std::string LastCall)
    :
    BFD("MalformedArchive", LastCall)
  { }

::BFP::Exception::BFD::FileNotRecognized::FileNotRecognized(::std::string LastCall)
    :
    BFD("FileNotRecognized", LastCall)
  { }

::BFP::Exception::BFD::FileAmbiguouslyRecognized::FileAmbiguouslyRecognized(::std::string LastCall)
    :
    BFD("FileAmbiguouslyRecognized", LastCall)
  { }

::BFP::Exception::BFD::NoContents::NoContents(::std::string LastCall)
    :
    BFD("NoContents", LastCall)
  { }

::BFP::Exception::BFD::NonrepresentableSection::NonrepresentableSection(::std::string LastCall)
    :
    BFD("NonrepresentableSection", LastCall)
  { }

::BFP::Exception::BFD::NoDebugSection::NoDebugSection(::std::string LastCall)
    :
    BFD("NoDebugSection", LastCall)
  { }

::BFP::Exception::BFD::BadValue::BadValue(::std::string LastCall)
    :
    BFD("BadValue", LastCall)
  { }

::BFP::Exception::BFD::FileTruncated::FileTruncated(::std::string LastCall)
    :
    BFD("FileTruncated", LastCall)
  { }

::BFP::Exception::BFD::FileTooBig::FileTooBig(::std::string LastCall)
    :
    BFD("FileTooBig", LastCall)
  { }

::BFP::Exception::BFD::InvalidErrorCode::InvalidErrorCode(::std::string LastCall)
    :
    BFD("InvalidErrorCode", LastCall)
  { }

::BFP::Exception::BFD::WrongObjectFormat::WrongObjectFormat(::std::string LastCall)
    :
    BFD("WrongObjectFormat", LastCall)
  { }

::BFP::Exception::BFD::MissingDso::MissingDso(::std::string LastCall)
    :
    BFD("MissingDso", LastCall)
  { }

::BFP::Exception::BFD::OnInput::OnInput(::std::string LastCall)
    :
    BFD("OnInput", LastCall)
  { }

::BFP::Exception::BFD::IteratorExpected::IteratorExpected(::std::string LastCall)
    :
    BFD("IteratorExpected", LastCall)
  { }

::BFP::Exception::Plugins::Plugins(
    const char *ex_msg,
    ::std::string LastCall)
  {
    EX_HEADER(msg, LastCall, ex_msg);
    msg += "\n" + get_backtrace();
  }

::BFP::Exception::Plugins::FileIsNotADir::FileIsNotADir(::std::string LastCall)
    :
    Plugins(
        "Path provided to Plugin Manager is not a directory (should be directory with plugins)",
        LastCall)
  { }

::BFP::Exception::Plugins::StageDirExpected::StageDirExpected(::std::string LastCall)
    :
    Plugins("In plugin directory are stage directories expected", LastCall)
  { }

::BFP::Exception::Plugins::DLError::DLError(::std::string LastCall)
    :
    Plugins("Plugin's array was not found in plugin!", LastCall)
  { }
