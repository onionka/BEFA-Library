//
// Created by miro on 1/25/17.
//

#ifndef BEFA_ASSERT_HPP
#define BEFA_ASSERT_HPP

/**
 * Simple macro for assertion via exception
 *
 * @param cond if this is false, raise runtime error
 * @param msg is message passed into std::runtime_error exception
 * @raises std::runtime_error
 */
#define assert_ex(cond, msg) \
  assert_ex_impl(cond, msg, __LINE__, __FILE__)

// To disable dynamic, exception-based assertion
// use compile flag -DNASSERT_EX=1
// to explicitly enable assert (it is enabled by default)
// use compile flag -DNASSERT_EX=0
#if !defined(NASSERT_EX) || NASSERT_EX == 0
# define assert_ex_impl(cond, msg, line, file) do { \
  if (!(cond)) throw std::runtime_error( \
    std::to_string(line) + ":" + file + ":" + msg \
  ); \
} while(0)
#else  // NASSERT_EX
# define assert_ex_impl(cond, msg, line, file) do {} while (0)
#endif  // NASSERT_EX

#endif //BEFA_ASSERT_HPP

