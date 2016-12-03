#include <gtest/gtest.h>
#include <algorithm>
#include <befa/utils/observer.hpp>

#include "fixtures.hpp"

#define CREATE_TEST_FIXTURE(name, filename)           \
  class name: public ::testing::Test {                \
  public:                                             \
    name() : file(ExecutableFile::open(filename)) {}  \
    virtual ~name() {}                                \
   protected:                                         \
    ExecutableFile file;                              \
  };

struct AssemblySequenceFinder : Observer<
    const ExecutableFile::instruction_type &
> {
  using observer_type = Observer<const ExecutableFile::instruction_type &>;
  using subscription_type = observer_type::subscription_type;

  AssemblySequenceFinder(
      // in
      const std::vector<std::string> &instr_sequence,
      size_t seq_index,
      // out
      int &seq_found
  ) : instr_sequence(instr_sequence),
      seq_index(seq_index),
      seq_found(seq_found) {}

  void operator()(const ExecutableFile::instruction_type &instr) override {
    assert(subscription && "invalid pointer");
    assert(*subscription && "invalid subscription");
    if (instr_sequence[seq_index] != instr.parse()[0]) {
      subscription->unsubscribe();
    } else if (seq_index == instr_sequence.size() - 1) {
      ++seq_found;
      subscription->unsubscribe();
    }
    ++seq_index;
  }

  void register_subscription(
      std::shared_ptr<subscription_type> subscription
  ) override { this->subscription = subscription; }
 private:
  // in
  const std::vector<std::string> &instr_sequence;
  size_t seq_index;

  // out
  int &seq_found;

  // locals
  std::shared_ptr<subscription_type> subscription
      = std::make_shared<subscription_type>();
};


// ==========================================================================
CREATE_TEST_FIXTURE(
    SimpleFixture,
    "test_cases/simple/simple"
)

TEST_F(SimpleFixture, SimpleTest) {
  typedef ExecutableFile::instruction_type i_type;

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
  auto &assembly$ = file.disassembly();
  assembly$.subscribe([&](const i_type &instr) {
    if (instr_sequence[0] == instr.parse()[0]) {
      assembly$.subscribe(
          AssemblySequenceFinder(instr_sequence, 1, seq_found)
      );
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
  typedef ExecutableFile::instruction_type i_type;

  const std::vector<std::string> instr_sequence{
      "push",
      "inc",
      "pop"
  };

  int seq_found = 0;

  auto global_function$ = file.disassembly().conditional([&](i_type instr) {
    return *ptr_lock(instr.getParent()->getParent()) == "global_function";
  });

  global_function$.subscribe([&](i_type instr) {
    if (instr_sequence[0] == instr.parse()[0]) {
      global_function$.subscribe(
          AssemblySequenceFinder(instr_sequence, 1, seq_found)
      );
    }
  });

  file.runDisassembler();

  EXPECT_GT(seq_found, 0);
}

// ==========================================================================
