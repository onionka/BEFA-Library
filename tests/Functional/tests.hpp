/**
 * @file main.h
 * @author Miroslav Cibulka
 * @brief HERE goes all tests
 */

#ifndef BINARYFILEPARSER_TESTS_HPP
#define BINARYFILEPARSER_TESTS_HPP

#include "main.h"

DEFINE_TEST_SUIT(functional_suite) {
        DEFINE_TEST(Opening_libsample_so, {
                auto __bfd = ::BFP::BFD::get_unique_instance();
                try {
                    __bfd->Open(*argv, "elf64-x86-64");
                    __bfd->Close();
                    return 0;
                } catch (::BFP::base_exception &ex) {
                    ::std::cerr << ex.what();
                    return 1;
                }
        }),
        DEFINE_TEST(Opening_libsample_so_FAIL, {
                auto __bfd = ::BFP::BFD::get_unique_instance();
                try {
                    __bfd->Open("Not Existing Binary", "elf64-x86-64");
                    return 1;
                } catch (::BFP::base_exception &ex) {
                    return 0;
                }
        }),
        DEFINE_TEST(Closing_BFD, {
                auto __bfd = ::BFP::BFD::get_unique_instance();
                try {
                    __bfd->Close();
                    return 0;
                } catch (::BFP::base_exception &ex) {
                    ::std::cerr << ex.what();
                    return 1;
                }
        }),
        DEFINE_TEST(Dealocate_bfp_bfd, {
                auto __bfd = ::BFP::BFD::get_unique_instance();
                try {
                    delete __bfd;
                    return 0;
                } catch (::BFP::base_exception &ex) {
                    ::std::cerr << ex.what();
                    return 1;
                }
        }),
};


#endif //BINARYFILEPARSER_TESTS_HPP
