/** Binary File Parser
 * @file bfp.hpp
 * @author Miroslav Cibulka
 * @brief Main header for BFP
 *        TODO: Description
 */

#ifndef BINARY_FILE_PARSER_BFP_HPP
#define BINARY_FILE_PARSER_BFP_HPP

#include <string.h>
#include <bfd.h>
#include <bfp/exception.hpp>

namespace BFP {

    /** Singleton of Binary File Descriptor
     * @brief This represents BFD class
     */
    class BFD {
    public:
        static BFD *get_unique_instance();

        void Open(const char *_file_name, const char *_target);

        ~BFD();

    private:
        BFD();

    private:
        bfd *fd;
    };
}

#endif //BINARY_FILE_PARSER_BFP_HPP
