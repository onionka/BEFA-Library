//
// Created by miro on 1/25/17.
//

#ifndef BEFA_ASSERT_HPP
#define BEFA_ASSERT_HPP

/**
 * Simple macro for assertion by exception
 *
 * @param cond if this is false, raise runtime error
 * @param msg is message passed into std::runtime_error exception
 * @raises std::runtime_error
 */
#define assert_ex(cond, msg) do { \
    if (!(cond)) throw std::runtime_error(msg); \
  } while(0)

#endif //BEFA_ASSERT_HPP
