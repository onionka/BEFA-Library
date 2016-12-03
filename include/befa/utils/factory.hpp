//
// Created by miro on 11/30/16.
//

#ifndef BEFA_FACTORY_HPP
#define BEFA_FACTORY_HPP

#include <vector>

/**
 * Template class for all factories
 */
template<typename /*T*/>
struct Factory {
// No need for virtual method, specialization will do the trick
//  Requirements:
//  "virtual" std::shared_ptr<T> create(
//      const std::vector<std::string> &input
//  ) = 0;
};

#endif //BEFA_FACTORY_HPP
