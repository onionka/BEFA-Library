/**
 * @file sample.cpp
 * @author Miroslav Cibulka
 * @brief This is plugin for testing purposes
 */

#include "sample.h"
#include <iostream>

Sample::Sample() {
    ::std::cout << "Loading Sample plugin" << ::std::endl;
}

const char *Sample::getVersion() const {
    return version;
}

const char *Sample::getName() const {
    return name;
}

Sample::~Sample() {
    ::std::cout << "Unloading Sample plugin" << ::std::endl;
}