//
// Created by onionka on 30/06/15.
//

#ifndef BINARYFILEPARSER_BFP_HPP
#define BINARYFILEPARSER_BFP_HPP


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

    class VSection {
    public:
        virtual const char *type() const = 0;

        virtual const char *getName() const = 0;
    };
}

#endif //BINARYFILEPARSER_BFP_HPP
