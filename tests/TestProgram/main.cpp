/**
 * @file test.cpp
 * @author Miroslav Cibulka
 * @brief This is sample test which checks only
 *        if BFD opens file correctly
 */

#include <iostream>
#include <string.h>
#include <bfp.hpp>


int main(
    int args,
    const char **argv)
  {
    auto _bfd = ::BFP::BFD::get_unique_instance();
    try
      {
        auto file = _bfd->Open(*argv, NULL);
        /// do some stuff with file ...
      } catch (::BFP::base_exception &ex)
      {
        ::std::cerr << ex.what();
        return EXIT_FAILURE;
      }
    delete _bfd;
    return EXIT_SUCCESS;
  }