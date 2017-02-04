#include <gtest/gtest.h>
#include <algorithm>

#include "fixtures.hpp"

#define CREATE_TEST_FIXTURE(name, filename)           \
  class name: public ::testing::Test {                \
  public:                                             \
    name() : file(ExecutableFile::open(filename)) {}  \
    virtual ~name() {}                                \
   protected:                                         \
    ExecutableFile file;                              \
  };

using ir_t = ExecutableFile::inst_t;

auto create_mapper(
    rxcpp::composite_subscription subscription,
    // in
    const std::vector<std::string> &instr_sequence,
    size_t seq_index
) {
  return [=, &instr_sequence](
      ir_t::c_info::ref instr
  ) mutable -> bool {
    // stop iteration
    if (seq_index >= instr_sequence.size() ||
        instr_sequence[seq_index] != instr.getName()) {
      subscription.unsubscribe();
      // the sequence has been found
    } else if (seq_index == instr_sequence.size() - 1) {
      subscription.unsubscribe();
      return true;
    }
    ++seq_index;
    return false;
  };
}

// ==========================================================================
CREATE_TEST_FIXTURE(
    SimpleFixture,
    "test_cases/simple/simple"
)

TEST_F(SimpleFixture, SimpleTest) {
  const std::vector<std::string> instr_sequence{
      "push",
      "push",
      "mov",
      "inc",
      "mov",
      "xor",
      "mov",
      "mov",
      "pop",
      "pop"
  };

  int seq_found = 0;
  auto assembly$ = file.disassembly();
  assembly$.subscribe([&](ir_t::c_info::ref instr) {
    if (instr_sequence[0] == instr.getName()) {
      auto subscription = rxcpp::composite_subscription();
      subscription = assembly$.map(
                                  create_mapper(
                                      subscription,
                                      instr_sequence,
                                      1
                                  )
                              )
                              .subscribe([&seq_found](bool result) {
                                if (result) ++seq_found;
                              });
    }
  });

  file.runDisassembler();

  EXPECT_GT(seq_found, 0);
}


// ==========================================================================
CREATE_TEST_FIXTURE(
    GlobalFunctionFixture,
    "test_cases/global_function/global_function"
)

TEST_F(GlobalFunctionFixture, GlobalFunctionTest) {
  const std::vector<std::string> instr_sequence{
      "push",
      "inc",
      "pop"
  };

  int seq_found = 0;

  auto global_function$ = file
      .disassembly()
      .filter([&](ir_t::c_info::ref instr) {
        return *ptr_lock(
            instr.getParent()
                 ->getParent()
        ) == "global_function";
      });

  global_function$.subscribe([&](ir_t::c_info::ref instr) {
    if (instr_sequence[0] == instr.getName()) {
      auto subscription = rxcpp::composite_subscription();
      subscription = global_function$
          .map(create_mapper(
              subscription,
              instr_sequence,
              1
          ))
          .subscribe([&seq_found](bool result) {
            if (result) ++seq_found;
          });
    }
  });

  file.runDisassembler();

  EXPECT_GT(seq_found, 0);
}

// ==========================================================================
