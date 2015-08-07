/**
 * @file test.cpp
 * @author Miroslav Cibulka
 * @brief This is sample test which checks only
 *        if BFD opens file correctly
 */

#include <iostream>
#include <bfp.hpp>


int main(
    int args,
    const char **argv)
  {
    auto _bfd = ::BFP::BFD::get_unique_instance();
    try
      {
        auto file = _bfd->Open(*argv, "");
        if (file == nullptr)
          {
            BFP_ASSERT();
            return EXIT_FAILURE;
          }
        for (auto &_sec : *file)
          for (auto &_sym : _sec)
            {
              /// process symbol
            }
      }
    catch (::BFP::base_exception &ex)
      {
        ::std::cerr << ex.what();
        return EXIT_FAILURE;
      }
    delete _bfd;
    return EXIT_SUCCESS;
  }