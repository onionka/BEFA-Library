/**
 * @file main.c
 * @author Miroslav Cibulka
 * @brief Functional test main function
 */

#include "main.h"
#include "tests.hpp"

int main(int argc, char **argv) {
    int return_code = 0,
        total = 0;
    for (auto test : functional_suite) {
        if (test(argv))
            ++return_code;
        ++total;
    }
    if (return_code)
        ::std::cout << return_code << "/" << total << " " << RED << "failed!" << NORMAL << ::std::endl;
    else
        ::std::cout << "All tests " << GREEN << "passed" << NORMAL << ::std::endl;
    return return_code;
}