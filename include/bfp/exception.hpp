/**
 * @file exception.hpp
 * @author Miroslav Cibulka
 * @brief Exception system for Binary File Parser
 */

#ifndef BINARY_FILE_PARSER_EXCEPTION_HPP
#define BINARY_FILE_PARSER_EXCEPTION_HPP

#include <iostream>
#include <execinfo.h>
#include <dlfcn.h>
#include <cstring>
#include <bfd.h>

/** Use THIS instead of throw */
#define RAISE(ex) throw ex(std::string(__FILE__) + ":" + ::std::to_string(__LINE__) + ":" + __FUNCTION__ + "()")

namespace BFP {

    /** ::BFP::Exception
     * @brief Base class for all BFP exceptions (for exception isolation from system or similar)
     */
    class Exception : public ::std::exception {
    public:
        /** Exceptions system for BFD
         * @brief Basel class for BFD exceptions with function backtrack
         */
        class BFD;

        /** Exception system for Plugins
         * @brief Base class for plugin exceptions with function backtrack
         */
        class Plugins;

    protected:
        /** Returns function backtrace */
        ::std::string get_backtrace();
    };

    class Exception::BFD : public Exception {
    public:
        /** May be thrown on closing file */
        class Closing;

        /** May be thrown on opening file */
        class Opening;

        /**
         * @param ex_msg is message from thrower
         * @param LastCall is name of last called method inserted by macro RAISE
         */
        BFD(const char *ex_msg, ::std::string LastCall = "Unknown");

        const virtual char *what() const throw() {
            return msg.c_str();
        }
    private:
        ::std::string msg;
    };

    class Exception::Plugins  : public Exception {
    public:
        /** May be thrown on loading plugin */
        class LoadingPlugin;

        /** May be thrown on loading plugin file */
        class PluginsArrNotExists;

        /** May be thrown on creating instance of plugin class */
        class CreatingPluginInstance;

        /** May be thrown on creating instance of plugin class */
        class PluginsArrNotExists;

        /** May be thrown on creating instance of PluginManager */
        class FileIsNotADir;

        /** May be thrown on iterating Plugins directory */
        class StageDirExpected;

        /**
         * @param ex_msg is message from thrower
         * @param LastCall is name of last called method inserted by macro RAISE
         */
        Plugins(const char *ex_msg, ::std::string LastCall = "Unknown");

        const char virtual *what() const throw() {
            return msg.c_str();
        }

    private:
        ::std::string msg;
    };

    class Exception::BFD::Closing : public Exception::BFD {
    public:
        Closing(::std::string LastCall);
    };

    class Exception::BFD::Opening : public Exception::BFD {
    public:
        Opening(::std::string LastCall);
    };


    class Exception::Plugins::LoadingPlugin : public Exception::Plugins {
    public:
        LoadingPlugin(::std::string LastCall);
    };


    class Exception::Plugins::CreatingPluginInstance : public Exception::Plugins {
    public:
        CreatingPluginInstance(::std::string LastCall);
    };

    class Exception::Plugins::PluginsArrNotExists : public Exception::Plugins {
    public:
        PluginsArrNotExists(::std::string LastCall);
    };

    class Exception::Plugins::FileIsNotADir : public Exception::Plugins {
    public:
        FileIsNotADir(::std::string LastCall);
    };

    class Exception::Plugins::StageDirExpected : public Exception::Plugins {
    public:
        StageDirExpected(::std::string LastCall);
    };

}
#endif //BINARY_FILE_PARSER_EXCEPTION_HPP
