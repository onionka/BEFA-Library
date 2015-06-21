/**
 * @file plugin_test.cpp
 * @author Miroslav Cibulka
 * @brief Basic test of sample plugin (load and plugin function call)
 */

#include <iostream>
#include <bfp/plugin_manager.hpp>
#include <bfp.hpp>
#include <vector>

/**
 * @brief Factory for making plugin from Sample shared library
 */
class SamplePluginFactory : public VFactory {
public:
    /** Init sample plugin factory
     * @brief This should load shared library and load creator function
     * @param plugin_name should be name of class that we want to load
     */
    SamplePluginFactory() {
        handle = dlopen(getLibName().c_str(), RTLD_LAZY);
        if (handle == NULL)
            RAISE(::BFP::Exception::LoadingPlugin);

        const char **_plugins = reinterpret_cast<const char **>(dlsym(handle, "plugins"));
        if (_plugins == NULL)
            RAISE(::BFP::Exception::PluginsArrNotExists);
        for (; *_plugins != NULL; ++_plugins)
            plugins.push_back(::std::string(*_plugins));
    }

    /** Creates instance of plugin
     * @return Instance of plugin class
     */
    virtual VComponent *create(::std::string _plug) {
        creator = reinterpret_cast<VComponent *(*)()>(
                dlsym(handle, (::std::string("creator_") + _plug).c_str()));
        if (creator == NULL)
            RAISE(::BFP::Exception::CreatingPluginInstance);
        return creator();
    }

    /** Returns name of loaded shared library */
    virtual const ::std::string getLibName() const {
        return shared_library;
    }

    /** Returns list of plugins in file */
    virtual const ::std::vector<::std::string> getPlugins() const {
        return plugins;
    }

    /** Closes library handle */
    ~SamplePluginFactory() {
        dlclose(handle);
    }
private:
    void *handle;

    VComponent *(*creator)();

    ::std::vector<::std::string> plugins;

    const ::std::string shared_library = "../plugin/libsample.so";
};

int main() {
    try {
        VFactory *PluginFactory = new SamplePluginFactory();
        VComponent *Sample = PluginFactory->create("Sample");
        ::std::cout << "Plugin: " <<
                Sample->getName() << "-" << Sample->getVersion() <<
                " was loaded!" << ::std::endl;
    } catch (::BFP::Exception &ex) {
        ::std::cerr << ex.what();
    }
    return EXIT_SUCCESS;
}