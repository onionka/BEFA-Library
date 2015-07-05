//
// Created by onionka on 04/07/15.
//

#include "main.h"

int main() {
    int return_code = 0, total = 0;
    for (auto test : functional_suite) {
        if (test())
            ++return_code;
        ++total;
    }
    if (return_code) {
        ::std::cout << return_code << "/" << total << " " << RED << "failed!" << NORMAL << ::std::endl;
    }
    else {
        ::std::cout << "All tests " << GREEN << "passed" << NORMAL << ::std::endl;
    }
    return return_code;
}