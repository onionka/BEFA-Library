#include <iostream>
#include <string.h>
#include "../bfp/bfp.hpp"


int main(int args, const char **argv) {
    auto fd = BFP::BFD::get_unique_instance();
    try {
        if (args == 2) {
            fd->Open(*(argv + 1), NULL);
        } else {
            ::std::cout << *argv << " <FILE_NAME>" << ::std::endl;
            return EXIT_SUCCESS;
        }
    } catch (::BFP::Exception &ex) {
        ::std::cerr << ex.what();
        return EXIT_SUCCESS;
    }
    delete fd;
    return EXIT_FAILURE;
}