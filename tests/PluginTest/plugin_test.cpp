/**
 * @file plugin_test.cpp
 * @author Miroslav Cibulka
 * @brief Basic test of sample plugin (load and plugin function call)
 */

#include <iostream>
#include <bfp.hpp>

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
        ::BFP::VFactory *_factory = new ::BFP::PluginFactory("./libsample.so");
        ::BFP::Stage *PluginStage = new ::BFP::Stage();
        PluginStage->addPluginFactory(_factory);
        ::BFP::VSection *_section = new ::Section();
        PluginStage->deployPlugins(_section);
        delete PluginStage; delete _section; delete _factory;
    } catch (::std::exception &ex) {
        ::std::cerr << ex.what();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}