//
// Created by miro on 1/25/17.
//

#ifndef BEFA_ASSERT_HPP
#define BEFA_ASSERT_HPP

#define BACKWARD_HAS_BFD 1

#include <regex>
#include <cassert>
#include "rxcpp/rx.hpp"

#include "backward.hpp"
// bfd.hpp and backward.hpp defines the same macro
#undef GCC_VERSION


namespace details {
template<typename ValT, typename SrcT>
ValT fetch_result(rxcpp::observable<ValT, SrcT> o$) {

  ValT result = "";
  o$.subscribe([&result](ValT str) { result = str; });
  return result;
}
}  // namespace details

namespace std {
template<typename T>
inline string to_string(vector<T> vec) {
  int index = 0;

  return details::fetch_result(
      rxcpp::sources::iterate(vec)
          .map([&index](const T &val) {
            return std::to_string(index++) + ": " + to_string(val);
          })
          .reduce(std::string("["), [](std::string acc, std::string val) {
            return acc + (acc != "[" ? ", " : "") + val;
          })
          .map([](std::string res) {
            return res + "]";
          })
  );
}

template<typename T, typename KeyT>
inline string to_string(map<T, KeyT> _map) {
  using pair_t = typename map<T, KeyT>::value_type;

  int index = 0;
  return details::fetch_result(
      rxcpp::sources::iterate(_map)
          .map([&index](const pair_t &val) {
            return to_string(val.first) + ": " + to_string(val.second);
          })
          .reduce(string("{"), [](string acc, string val) {
            return acc + (acc != "{" ? ", " : "") + val;
          })
          .map([](string res) { return res + "}"; })
  );
}
}  // namespace std

/**
 * Simple macro for assertion via exception
 * Prints stack trace with CLion friendly 'file:line' references
 *
 * @param cond if this is false, raise runtime error
 * @param msg is message passed into std::runtime_error exception
 * @raises std::runtime_error
 */
#define assert_ex(cond, msg) \
  assert_ex_impl(cond, msg, __LINE__, __FILE__)

namespace details {
/**
 * Replaces input buffer of c-string
 *
 * @param in is a c-string buffer
 * @param size is size of buffer
 * @return string with CLion friendly 'file:line' format
 */
inline std::string replace(const char *in, size_t size) {
  std::regex regex("Source \"([^\"]+)\", line ([0-9]+)");
  std::string output;
  std::regex_replace(
      std::back_inserter(output), in, in + size,
      regex, "$1:$2");
  return output;
}

/**
 * Creates nice stack trace
 *
 * @return stack trace with CLion friendly 'file:line' format
 *         and a snippets from source code (if '-g' compiler flag is on)
 */
inline std::string stack_trace() {
  using namespace backward;

  constexpr size_t buffer_size = 1000000;
  constexpr uint8_t buffer[buffer_size] = {0};
  constexpr char buffer_str[buffer_size] = {0};

  StackTrace st;
  Printer p;
  FILE *str_file = NULL;

  st.load_here(10);
  p.object = true;
  p.snippet = true;
  p.color = true;
  p.address = true;

  // memory file
  str_file = fmemopen((void *) buffer, buffer_size, "w+");

  // as a replacement without pretty stacktrace ...
  assert(
      str_file &&
      "creating stacktrace failed - not enough memory"
  );

  { // print stack trace to memory file
    long position = ftell(str_file);
    p.print(st, str_file);
    fflush(str_file);
    fseek(str_file, position, SEEK_SET);
  }
  { // read stack trace from file
    size_t file_read = fread(
        (void *) buffer_str, 1,
        buffer_size, str_file
    );
    fclose(str_file);

    // Mark end of string (nasty cast to non-const)
    ((uint8_t *) buffer_str)[file_read] = 0;

    // replace files and line into runtime_error format
    return replace(buffer_str, file_read);
  }
}
}  // namespace details

// To disable dynamic, exception-based assertion
//    use compile flag -DNASSERT_EX=1
// to explicitly enable assert (it is enabled by default)
//    use compile flag -DNASSERT_EX=0
#if !defined(NASSERT_EX) || NASSERT_EX == 0
# define assert_ex_impl(cond, msg, line, file) do { \
  if (!(cond)) { \
    fprintf(stderr, "%s\n", ::details::stack_trace().c_str()); \
    throw std::runtime_error( \
        file + std::string(":") + std::to_string(line) + "\n\t" + msg \
    ); \
}} while(0)
#else  // NASSERT_EX
// empty instruction as a replacement for release
# define assert_ex_impl(cond, msg, line, file) do {} while (0)
#endif  // NASSERT_EX

#endif //BEFA_ASSERT_HPP

