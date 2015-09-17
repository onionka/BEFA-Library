//
// Created by onionka on 17/09/15.
//

#include <bfp.hpp>
#include <sstream>
#include <bits/stl_algo.h>
#include <boost/tokenizer.hpp>
#include <fstream>


::std::string normalize_path(::std::string _s)
  {
    if (_s.size() <= 20)
      { return _s; }
    ::std::string out;
    ::std::copy(_s.begin(), _s.begin() + 10, ::std::back_inserter(out));
    out += "...";
    ::std::copy(_s.end() - 10, _s.end(), ::std::back_inserter(out));
    return out;
  }

::std::string getLine(
    ::std::ifstream &s,
    size_t n)
  {
    s.seekg(::std::ios_base::beg);
    ::std::string line;
    while (::std::getline(s, line))
      if (--n == 0)
        return line;
    return ::std::string("");
  }

typedef ::boost::tokenizer<
    ::boost::char_separator<char> > _LineTokenizerT;


template<
    typename _Input_Vector,
    typename _Predicate,
    typename _Output_Vector = ::std::vector<
        typename ::std::remove_reference<
            _Input_Vector>::type::value_type>>
  _Output_Vector collect(
      _Input_Vector &&_vector,
      _Predicate &&_pred)
    {
      _Output_Vector out;
      ::std::copy_if(::std::begin(_vector), ::std::end(_vector),
                     ::std::back_inserter(out),
                     ::std::forward<_Predicate>(_pred));
      return ::std::move(out);
    }


int find_uses(
    ::bfp::Section &text,
    ::bfp::Symbol &sym)
  {
    size_t last_line = 0;
    for (auto &_s : text)
      {
        for (auto &_malloc_call : collect(_s, [&sym](::bfp::Instruction &_i)
          {
            std::stringstream str;
            str << std::hex << sym.getValue();
            return ::std::string(_i.getSignature()).find(str.str()) !=
                   ::std::string::npos;
          }))
          {
            ::bfp::LineInfo lineInfo = _malloc_call.getLineInfo();
            auto line_num = lineInfo.get_linenumber();
            if (line_num == last_line)
              continue;
            ::std::cout << "@" << normalize_path(lineInfo.get_filename()) <<
            ":" << lineInfo.get_functionname() << "():" << line_num <<
            ::std::endl << "..." << ::std::endl;
            ::std::ifstream stream(lineInfo.get_filename());
            for (size_t i = line_num - 2;
                 i < line_num + 2;
                 ++i)
              ::std::cout << getLine(stream, i) << ::std::endl;
            last_line = line_num;
          }
      }
    ::std::cout << "..." << ::std::endl;
    return EXIT_SUCCESS;
  }

int find_calls(
    bfp::File &file,
    ::std::string _what)
  {
    for (auto &sec : file)
      {
        auto sym = ::std::find_if(::std::begin(sec), ::std::end(sec),
                                  [&_what](::bfp::Symbol &_s)
                                    {
                                      return _s.getName() == _what;
                                    });
        if (sym != ::std::end(sec))
          return find_uses(*::std::find_if(::std::begin(file), ::std::end(file),
                                           [](::bfp::Section &_sec)
                                             { return _sec == ".text"; }),
                           *sym);
      }
    return EXIT_SUCCESS;
  }

int main(
    int argc,
    const char **args)
  {
    try
      {
        if (argc != 2)
          throw ::std::runtime_error("Exactly one parameter is needed!");
        auto parser = ::bfp::Parser::get_unique_instance();
        auto file = parser.Open(args[1], "");
        ::std::cout << "Uses of malloc:" << ::std::endl;
        find_calls(file, "malloc@plt");
        ::std::cout << ::std::endl << "Uses of free:" << ::std::endl;
        find_calls(file, "free@plt");
        ::std::cout << ::std::endl << "Uses of new:" << ::std::endl;
        find_calls(file, "_Znam@plt");
        ::std::cout << ::std::endl << "Uses of new[]:" << ::std::endl;
        find_calls(file, "_Znwm@plt");
        ::std::cout << ::std::endl << "Uses of delete:" << ::std::endl;
        find_calls(file, "_ZdlPvm@plt");
        ::std::cout << ::std::endl << "Uses of delete[]:" << ::std::endl;
        find_calls(file, "_ZdaPv@plt");
      }
    catch (::std::exception &ex)
      {
        ::std::cerr << ex.what() << ::std::endl;
        return EXIT_FAILURE;
      }
    return EXIT_SUCCESS;
  }