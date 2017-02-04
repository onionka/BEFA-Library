#include <gtest/gtest.h>
#include "fixtures.hpp"
#include "befa/utils/backward.hpp"

const char *file_name = nullptr;

int main(int argc, char **argv) {
  using namespace backward;

  SignalHandling sh;
  file_name = *(argv + 1);
  assert_ex(file_name , "expecting file name parameter");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}