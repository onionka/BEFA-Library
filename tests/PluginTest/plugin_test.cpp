/**
 * @file plugin_test.cpp
 * @author Miroslav Cibulka
 * @brief Basic test of sample plugin (load and plugin function call)
 */

#include <iostream>
#include <bfp/plugin_manager.hpp>

/**
 * @brief Factory for making plugin from Sample shared library
 */
class SamplePluginFactory : public ::BFP::VFactory {
public:
    /** Init sample plugin factory
     * @brief This should load shared library and load creator function
     * @param plugin_name should be name of class that we want to load
     */
    SamplePluginFactory() {
        handle = dlopen(getLibName().c_str(), RTLD_LAZY);
        if (handle == NULL)
            RAISE(::BFP::Exception::Plugins::LoadingPlugin);

        const char **_plugins = reinterpret_cast<const char **>(dlsym(handle, "plugins"));
        if (_plugins == NULL)
            RAISE(::BFP::Exception::Plugins::PluginsArrNotExists);
        for (; *_plugins != NULL; ++_plugins)
            plugins.push_back(::std::string(*_plugins));
    }

    /** Creates instance of plugin
     * @return Instance of plugin class
     */
    virtual ::BFP::VComponent *create(::std::string _plug) {
        creator = reinterpret_cast<::BFP::VComponent *(*)()>(
                dlsym(handle, (::std::string("creator_") + _plug).c_str()));
        if (creator == NULL)
            RAISE(::BFP::Exception::Plugins::CreatingPluginInstance);
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

    ::BFP::VComponent *(*creator)();

    ::std::vector<::std::string> plugins;

    const ::std::string shared_library = "libsample.so";
};

/** Example of section - will be expanded in future ... */
class Section : public ::BFP::VSection {
    virtual const char *type() const {
        return "Sample type";
    }

    virtual const char *getName() const {
        return "Sample Section name";
    }
};

int main(int argc, char **argv) {
    try {
        auto _factory = new SamplePluginFactory();
        ::BFP::Stage *PluginStage = new ::BFP::Stage(_factory);
        auto _section = new Section();
        PluginStage->deployPlugins(_section);
        delete PluginStage;
        delete _section;
        delete _factory;
    } catch (::std::exception &ex) {
        ::std::cerr << ex.what();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}