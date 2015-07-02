/**
 * @file sample.h
 * @author Miroslav Cibulka
 * @brief This is plugin for testing purposes
 */

#ifndef _PLUGIN_H
#define _PLUGIN_H

#include <bfp.hpp>

const char *plugins[] = {
        "Sample", NULL
};

class Sample : ::BFP::VComponent {
public:
    Sample();

    virtual void deploy(::BFP::VSection *arg);

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
