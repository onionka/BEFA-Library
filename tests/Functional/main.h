//
// Created by onionka on 04/07/15.
//

#ifndef BINARYFILEPARSER_MAIN_H
#define BINARYFILEPARSER_MAIN_H

#include <iostream>
#include <bfp.hpp>

#define GREEN           "\e[0;31m"
#define RED             "\e[0;32m"
#define RESET_COLOR     "\e[0m"

#define DEFINE_TEST(name)                   \
    int test_##name();                      \
    int name() {                            \
        std::cout << "Running test_"        \
                  << #name << "\t";         \
        if (!test_##name()) {               \
            std::cout << GREEN              \
                      << "OK"               \
                      << RESET_COLOR        \
                      << std::endl;         \
            return 0;                       \
        }                                   \
        else {                              \
            std::cout << RED                \
                      << "ERR"              \
                      << RESET_COLOR        \
                      << std::endl;         \
            return 1;                       \
        }                                   \
    }                                       \
    int test_##name()

int main();

DEFINE_TEST(sample) {
    auto bfd = ::BFP::BFD::get_unique_instance();
    try {
        bfd->Open("Not Existing Binary", "");
    } catch (::BFP::Exception &ex) {
        return 0;
    }
    return 1;
}

DEFINE_TEST(sample1) {
    return 0;
}

DEFINE_TEST(sample2) {
    return 0;
}

#endif //BINARYFILEPARSER_MAIN_H
