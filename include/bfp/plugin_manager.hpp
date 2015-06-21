/**
 * @file plugin_manager.hpp
 * @author Miroslav Cibulka
 * @brief Plugin manager for Binary file analyser
 */

#ifndef BINARYFILEPARSER_PLUGIN_MANAGER_HPP
#define BINARYFILEPARSER_PLUGIN_MANAGER_HPP

#include <dlfcn.h>
#include <bfp/exception.hpp>
#include <vector>

/** Component from which plugins are derived */
class VComponent {
public:
    /** Version of plugin */
    virtual const char *getVersion() const = 0;

    /** Name of plugin */
    virtual const char *getName() const = 0;

    /// TODO: Other info?
};

/** Factory from which Plugin factories are derived */
class VFactory {
public:
    /** Should creates instance of certain plugin class */
    virtual VComponent *create(::std::string) = 0;

    /** Should return shared library name */
    virtual const ::std::string getLibName() const = 0;

    /** Should return vector of plugin classes from shared library */
    virtual const ::std::vector<::std::string> getPlugins() const = 0;

    /** Virtual destructor for destructing derived objects */
    virtual ~VFactory() { }
};

/// TODO: Plugin Manager (client) class

///////////////////////////////////////////////////////////

#endif //BINARYFILEPARSER_PLUGIN_MANAGER_HPP
