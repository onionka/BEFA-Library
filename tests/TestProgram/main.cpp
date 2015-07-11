/**
 * @file test.cpp
 * @author Miroslav Cibulka
 * @brief This is sample test which checks only
 *        if BFD opens file correctly
 */

#include <iostream>
#include <string.h>
#include <bfp.hpp>

int main(int args, const char **argv) {
    auto fd = ::BFP::BFD::get_unique_instance();
    try {
        fd->Open(*argv, NULL);
    } catch (::std::exception &ex) {
        ::std::cerr << ex.what();
        return EXIT_FAILURE;
    }
    delete fd;
    return EXIT_SUCCESS;
}