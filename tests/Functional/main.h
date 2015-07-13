/**
 * @file main.h
 * @author Miroslav Cibulka
 * @brief Functional test defines
 */

#ifndef BINARYFILEPARSER_MAIN_H
#define BINARYFILEPARSER_MAIN_H

#include <iostream>
#include <bfp.hpp>
#include <iomanip>


#define GREEN           "\e[0;32m"
#define RED             "\e[0;31m"
#define NORMAL          "\e[0m"

#define DEFINE_TEST_SUIT(name)              \
    ::std::function<int(char **)> name[] =

#define DEFINE_TEST(name, code_block)       \
    [](char **argv) -> int {                \
        ::std::cout << ::std::left          \
                    << ::std::setfill('.')  \
                    << "Running test "      \
                    << ::std::setw(50)      \
                    << ::std::string(#name) \
                        + ": ";             \
        if (([](char **argv)-> int          \
                    code_block)(argv)) {    \
            ::std::cout << RED << " ERR"    \
                        << NORMAL           \
                        << ::std::endl;     \
            return 1;                       \
        } else {                            \
            ::std::cout << GREEN << " OK"   \
                        << NORMAL           \
                        << ::std::endl;     \
            return 0;                       \
        }                                   \
    }

#endif //BINARYFILEPARSER_MAIN_H
