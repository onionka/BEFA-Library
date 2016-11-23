#include <gtest/gtest.h>
#include <algorithm>

#include "../include/befa/utils/algorithms.hpp"
#include "fixtures.hpp"

#define CREATE_TEST_FIXTURE(name, filename) \
  class name: public ::testing::Test { \
   protected: \
    name() : file(ExecutableFile::open(filename)) {} \
    virtual ~name() {} \
    ExecutableFile file; \
  };

CREATE_TEST_FIXTURE(SimpleFixture, "test_cases/simple/simple")


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


struct AssemblySequence {
  AssemblySequence(size_t seq_index, bool &seq_found)
      : seq_index(seq_index), seq_found(seq_found) { }

  void operator()(const ExecutableFile::instruction_type &n_instr) {
    if (seq_index >= 10) return;
    if (instr_sequence[seq_index++] != n_instr.parse()[0])
      seq_index = 11;
    if (seq_index == 10) seq_found = true;
  }
 private:
  // in
  size_t seq_index;

  // out
  bool &seq_found;
};


TEST_F(SimpleFixture, SimpleTest) {
  typedef ExecutableFile::instruction_type i_type;

  bool seq_found = false;
  auto &assembly$ = file.assembly();
  assembly$.subscribe([&] (const i_type &instr) {
    if (instr_sequence[0] == instr.parse()[0]) {
      assembly$.subscribe(AssemblySequence(1, seq_found));
    }
  });

  file.runDisassembler();

  EXPECT_TRUE(seq_found);
}
