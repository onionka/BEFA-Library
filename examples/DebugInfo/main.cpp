/**
 * @file main.cpp
 * @author Miroslav Cibulka
 * @brief This is easy disassembler using BFP library
 */

#include <iostream>
#include <bfp.hpp>
#include <bits/stl_algo.h>
#include <boost/tokenizer.hpp>
#include <fstream>


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



template<
    typename _LineTokenizerT>
  auto getLine(
      _LineTokenizerT tokenizer,
      size_t n) -> typename _LineTokenizerT::iterator
    {
      for (auto _tok = ::std::begin(tokenizer);
           _tok != ::std::end(tokenizer);
           ++_tok)
        if (--n == 0)
          return _tok;
      return tokenizer.end();
    }
typedef ::boost::tokenizer<
    ::boost::char_separator<char> > _LineTokenizerT;


int main(
    int argc,
    const char *argv[])
  {
    try
      {
        if (argc == 1) throw ::std::runtime_error("File argument needed");
        auto parser = ::bfp::Parser::get_unique_instance();
        ::bfp::File file = parser.Open(argv[1], "");
        auto text = ::std::find_if(
            ::std::begin(file), ::std::end(file),
            [](::bfp::Section &_sec) -> bool
              { return _sec == ".text"; });

        ::std::string _last_fn;
        unsigned _last_ln = 0;
        for (::bfp::Symbol &sym : *text)
          {
            printf("%s <0x%08X>:\n",
                   sym.getName().c_str(), (unsigned) sym.getValue());

            if (sym.begin()->getLineInfo().get_filename() != nullptr)
              {
                ::std::ifstream stream(sym.begin()->getLineInfo().get_filename());
                ::std::string content ((std::istreambuf_iterator<char>(stream)),
                                       (std::istreambuf_iterator<char>()      ));
                _LineTokenizerT tok(content,
                                    ::boost::char_separator<char>("\n\r"));
                _LineTokenizerT::iterator _last_tok_ite = tok.begin();
                for (::bfp::Instruction &instr: sym)
                  {
                    auto _line = instr.getLineInfo();
                    if (_line.get_linenumber() != _last_ln ||
                        _last_fn != _line.get_filename())
                      {
                        auto _tok_ite = getLine(tok, _line.get_linenumber());
                        for (auto _b = _last_tok_ite; _b != _tok_ite; ++_b)
                          ::std::cout << *_b << ::std::endl;
                        _last_fn = _line.get_filename();
                        _last_ln = _line.get_linenumber();
                        _last_tok_ite = _tok_ite;
                      }
                    printf("        <0x%08X>: %-80.80s %s\n",
                           (unsigned) instr.getAddress(),
                           instr.getSignature(), instr.getBinary());
                  }
              }
          }
      }
    catch (::std::exception &e)
      {
        ::std::cerr << e.what() << ::std::endl;
        return EXIT_FAILURE;
      }
    return EXIT_SUCCESS;
  }
