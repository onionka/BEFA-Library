#include <gtest/gtest.h>
#include "fixtures.hpp"

const char *file_name = nullptr;

int main(int argc, char **argv) {
  file_name = *(argv + 1);
  assert(file_name && "expecting file name parameter");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}