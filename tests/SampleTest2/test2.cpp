/**
 * @file test.cpp
 * @author Miroslav Cibulka
 * @brief This is sample fail test which checks only
 *        if BFD fails to open file
 */

#include <iostream>
#include <string.h>
#include <bfp.hpp>
#include <bfp/exception.h>


int main(int args, const char **argv) {
    ::BFP::BFD *fd = ::BFP::BFD::get_unique_instance();
    try {
        fd->Open("NonExistingFile", NULL);
    } catch (::std::exception &ex) {
        ::std::cerr << ex.what();
        return EXIT_SUCCESS;
    }
    delete fd;
    return EXIT_FAILURE;
}