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
      } else
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
    BFD("Invalid operation", LastCall)
  { }

::BFP::Exception::BFD::NoSymbols::NoSymbols(::std::string LastCall)
    :
    BFD("Invalid operation", LastCall)
  { }

::BFP::Exception::BFD::NoArmap::NoArmap(::std::string LastCall)
    :
    BFD("No memory", LastCall)
  { }

::BFP::Exception::BFD::NoMoreArchivedFiles::NoMoreArchivedFiles(::std::string LastCall)
    :
    BFD("No symbols", LastCall)
  { }

::BFP::Exception::BFD::MalformedArchive::MalformedArchive(::std::string LastCall)
    :
    BFD("No armap", LastCall)
  { }

::BFP::Exception::BFD::FileNotRecognized::FileNotRecognized(::std::string LastCall)
    :
    BFD("No more archived files", LastCall)
  { }

::BFP::Exception::BFD::FileAmbiguouslyRecognized::FileAmbiguouslyRecognized(::std::string LastCall)
    :
    BFD("Malformed archive", LastCall)
  { }

::BFP::Exception::BFD::NoContents::NoContents(::std::string LastCall)
    :
    BFD("File not recognized", LastCall)
  { }

::BFP::Exception::BFD::NonrepresentableSection::NonrepresentableSection(::std::string LastCall)
    :
    BFD("File ambiguously recognized", LastCall)
  { }

::BFP::Exception::BFD::NoDebugSection::NoDebugSection(::std::string LastCall)
    :
    BFD("No contents", LastCall)
  { }

::BFP::Exception::BFD::BadValue::BadValue(::std::string LastCall)
    :
    BFD("Nonrepresentable section", LastCall)
  { }

::BFP::Exception::BFD::FileTruncated::FileTruncated(::std::string LastCall)
    :
    BFD("No debug section", LastCall)
  { }

::BFP::Exception::BFD::FileTooBig::FileTooBig(::std::string LastCall)
    :
    BFD("Bad value", LastCall)
  { }

::BFP::Exception::BFD::InvalidErrorCode::InvalidErrorCode(::std::string LastCall)
    :
    BFD("File truncated", LastCall)
  { }

::BFP::Exception::BFD::WrongObjectFormat::WrongObjectFormat(::std::string LastCall)
    :
    BFD("File is too big", LastCall)
  { }

::BFP::Exception::BFD::MissingDso::MissingDso(::std::string LastCall)
    :
    BFD("Input error", LastCall)
  { }

::BFP::Exception::BFD::OnInput::OnInput(::std::string LastCall)
    :
    BFD("Undefined BFD error", LastCall)
  { }

::BFP::Exception::BFD::IteratorExpected::IteratorExpected(::std::string LastCall)
    :
    BFD("Iterator is expected!", LastCall)
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
