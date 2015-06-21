//
// Created by onionka on 19/06/15.
//

#ifndef _PLUGIN_H
#define _PLUGIN_H

#include "plugin_manager.hpp"

class Sample : VComponent {
public:
    Sample();

    virtual const char *getVersion() const;

    virtual const char *getName() const;

    ~Sample();
private:
    const char *version = "0.0.1";

    const char *name = "Sample Plugin";
};

extern "C" Sample *creator_Sample() {
    return new Sample();
}

#endif //_PLUGIN_H
