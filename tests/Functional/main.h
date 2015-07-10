//
// Created by onionka on 04/07/15.
//

#ifndef BINARYFILEPARSER_MAIN_H
#define BINARYFILEPARSER_MAIN_H

#include <iostream>
#include <bfp.hpp>

#define GREEN           "\e[0;32m"
#define RED             "\e[0;31m"
#define NORMAL          "\e[0m"

#define DEFINE_TEST_SUIT(name)              \
    ::std::function<int()> name[] =

#define DEFINE_TEST(name, code_block)       \
    []() -> int {                           \
        std::cout << "Running test "        \
                  << #name << ":\t";        \
        if (([]()-> int code_block)()) {    \
            ::std::cout << RED << "ERR"     \
                        << NORMAL           \
                        << ::std::endl;     \
            return 1;                       \
        } else {                            \
            ::std::cout << GREEN << "OK"    \
                        << NORMAL           \
                        << ::std::endl;     \
            return 0;                       \
        }                                   \
    }

int main();

DEFINE_TEST_SUIT(functional_suite) {
    DEFINE_TEST(sample, {
        auto bfd = ::BFP::BFD::get_unique_instance();
        try {
            bfd->Open("Not Existing Binary", "");
        } catch (::BFP::Exception &ex) {
            return 0;
        }
        return 1;
    }),
    DEFINE_TEST(sample1, {
        return 0;
    }),
    DEFINE_TEST(sample2, {
        return 0;
    })
};
#endif //BINARYFILEPARSER_MAIN_H
