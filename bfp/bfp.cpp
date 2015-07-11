/** Binary File Parser
 * @file bfp.cpp
 * @author Miroslav Cibulka
 * @brief Module where Binary File Descriptor is abstracted
 */

#include <bfp.hpp>

namespace BFP {
    BFD *BFD::get_unique_instance() {
        static BFD *instance = nullptr;
        if (instance == nullptr) {
            instance = new BFD();
        }
        return instance;
    }

    void BFD::Open(const char *_file_name, const char *_target) {
        if ((fd = bfd_openr(_file_name, _target)) == NULL)
            RAISE(::BFP::Exception::BFD::Opening);
        else
            open = true;
    }

    void BFD::Close() {
        if (open && fd != NULL && !bfd_close(fd)) {
            RAISE(::BFP::Exception::BFD::Closing);
        }
    }

    BFD::~BFD() {
        BFD::Close();
    }

    BFD::BFD() :
            open {false}
    {
        bfd_init();
    }
}