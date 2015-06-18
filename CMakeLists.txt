cmake_minimum_required(VERSION 2.8)
project(BinaryFileParser)
include_directories(bfd)
add_subdirectory(bfp)
enable_testing()

add_subdirectory(tests)