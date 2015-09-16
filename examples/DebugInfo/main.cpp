/**
 * @file main.cpp
 * @author Miroslav Cibulka
 * @brief This is easy disassembler using BFP library
 */

#include <iostream>
#include <bfp.hpp>
#include <bits/stl_algo.h>


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


int main(
    int argc,
    const char *argv[])
  {
    try
      {
        if (argc == 1) throw ::std::runtime_error("File argument needed");
        auto parser = ::bfp::Parser::get_unique_instance();
        auto file = parser.Open(argv[1], "");
        auto _debug_secs = collect(file, [](::bfp::Section &_sec) -> bool
          { return _sec.getName().find("debug") != ::std::string::npos; });

        bool was_line = false;
        for (::bfp::Section &sec : _debug_secs)
          for (::bfp::Symbol &sym : sec)
            {
              printf("%s <0x%08X>:\n    %s <0x%08X>:\n        ",
                     sec.getName().c_str(), (unsigned) sec.getAddress(),
                     sym.getName().c_str(), (unsigned) sym.getValue());
              for (auto i = 0;
                   i < sym.size();
                   ++i)
                {
                  if (sym.getRawData()[i] == 0 && !was_line)
                    {
                      printf("\n        ");
                      was_line = true;
                      continue;
                    }
                  else if (isprint(sym.getRawData()[i]))
                    printf("%c", (char) (sym.getRawData()[i]));
                  else
                    printf(".");
                  was_line = false;
                }
              printf("\r");
            }
      }
    catch (::std::exception &e)
      {
        ::std::cerr << e.what() << ::std::endl;
        return EXIT_FAILURE;
      }
    return EXIT_SUCCESS;
  }
