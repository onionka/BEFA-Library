//
// Created by onionka on 19/06/15.
//

#ifndef BINARYFILEPARSER_PLUGIN_H
#define BINARYFILEPARSER_PLUGIN_H

#include <dlfcn.h>
#include "exception.hpp"

class VComponent {
public:
    virtual const char *getVersion() const = 0;

    virtual const char *getName() const = 0;
};

class VFactory {
public:
    virtual VComponent *create() = 0;

    virtual const char *getLibName() const = 0;

    virtual ~VFactory() { }
};

///////////////////////////////////////////////////////////

#endif //BINARYFILEPARSER_PLUGIN_H
