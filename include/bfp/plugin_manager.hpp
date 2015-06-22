/**
 * @file plugin_manager.hpp
 * @author Miroslav Cibulka
 * @brief Plugin manager for Binary file analyser
 */

#ifndef BINARYFILEPARSER_PLUGIN_MANAGER_HPP
#define BINARYFILEPARSER_PLUGIN_MANAGER_HPP

#include <dlfcn.h>
#include <vector>
#include <bfp.hpp>
#include <algorithm>

namespace BFP {
    /** Component from which plugins are derived */
    class VComponent {
    public:
        /** Deploy with argument */
        virtual void deploy(VSection *arg) = 0;

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

    /** Represent stage of analysis */
    class Stage {
    public:
        /** Binds factory to stage */
        Stage(VFactory *_plugin_factory) {
            PluginFactory = _plugin_factory;
            for (auto plugin: PluginFactory->getPlugins())
                Plugins.push_back(PluginFactory->create(plugin));
        }

        /** Deploys all loaded plugins except ignored */
        void deployPlugins(::BFP::VSection *section) {
            for (auto plugin: Plugins)
                if (::std::find(IgnoreList.begin(),
                                IgnoreList.end(),
                                ::std::string (plugin->getName())
                        ) == IgnoreList.end())
                    plugin->deploy(section);
        }

        /** Set ignore to certain library */
        void ignoreLibs(::std::string _ignore) {
            IgnoreList.push_back(_ignore);
        }

        /** Returns names of libraries that affects this stage of analysis */
        const ::std::vector<::std::string> getStageLibs() const {
            return StageLibs;
        }

        ~Stage() {
            for(auto plugin: Plugins)
                delete plugin;
        }
    private:
        ::std::vector<::std::string> StageLibs;
        ::std::vector<::std::string> IgnoreList;
        ::std::vector<VComponent *> Plugins;
        VFactory *PluginFactory;
    };

    /** Manages/deploys plugins at certain stages */
    class PluginManager : private ::std::vector<Stage *> {
    public:
        /** Creates vector of stages loaded from path
         * @param path to the shared libraries
         */
        PluginManager(::std::string path) {

        }

        ~PluginManager() { }
    private:
    };
}
///////////////////////////////////////////////////////////

#endif //BINARYFILEPARSER_PLUGIN_MANAGER_HPP
