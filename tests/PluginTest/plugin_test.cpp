//
// Created by onionka on 19/06/15.
//

#include <iostream>
#include <plugin_manager.hpp>
#include <bfp.hpp>

class SamplePluginFactory : public VFactory {
public:
    SamplePluginFactory(::std::string plugin_name) {
        handle = dlopen(getLibName(), RTLD_LAZY);
        if (handle == NULL)
            RAISE(::BFP::Exception::LoadingPlugin);
        creator = reinterpret_cast<VComponent *(*)()>(dlsym(handle, (std::string("creator_") + plugin_name).c_str()));
        if (creator == NULL)
            RAISE(::BFP::Exception::CreatingPluginInstance);
    }

    virtual VComponent *create() {
        return creator();
    }

    virtual const char *getLibName() const {
        return shared_library;
    }

    ~SamplePluginFactory() {
        dlclose(handle);
    }
private:
    void *handle;

    VComponent *(*creator)();

    const char *shared_library = "../plugin/libsample.so";
};

int main() {
    try {
        VFactory *PluginFactory = new SamplePluginFactory("Sample");
        VComponent *Sample = PluginFactory->create();
        ::std::cout << "Plugin: " << Sample->getName() << "-" <<
        Sample->getVersion() << " was loaded!" << ::std::endl;
    } catch (::BFP::Exception &ex) {
        ::std::cerr << ex.what();
    }
    return EXIT_SUCCESS;
}