/**
 * @file sample.h
 * @author Miroslav Cibulka
 * @brief This is plugin for testing purposes
 */

#ifndef _PLUGIN_H
#define _PLUGIN_H

#include <bfp/plugin_manager.hpp>

const char *list_of_plugins[] = {
        "Sample", NULL
};

extern "C" const char **plugins() {
    return list_of_plugins;
}

class Sample : VComponent {
public:
    Sample();

    virtual const char *getVersion() const;

    virtual const char *getName() const;

    ~Sample();
private:
    const char *version = "0.0.1";

    const char *name = "Sample_Plugin";
};

extern "C" Sample *creator_Sample() {
    return new Sample();
}

#endif //_PLUGIN_H
