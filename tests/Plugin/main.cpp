/**
 * @file main.cpp
 * @author Miroslav Cibulka
 * @brief This is plugin for testing purposes
 */

#include "main.h"
#include <iostream>


Sample::Sample()
  {
    ::std::cout << "Loading Sample plugin" << ::std::endl;
  }

const char *Sample::getVersion() const
  {
    return version;
  }

const char *Sample::getName() const
  {
    return name;
  }

void Sample::deploy()
  {
    ::std::cout << "Plugin was deployed: "
                << std::endl;
  }

Sample::~Sample()
  {
    ::std::cout << "Unloading Sample plugin" << ::std::endl;
  }