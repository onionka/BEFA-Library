cmake_minimum_required(VERSION 2.8)
project(BinaryFileParser)
if ("${USE_COMPILER}" STREQUAL "clang")
    set(CMAKE_C_COMPILER /usr/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
endif ()
#################################################################
# Finding BOOST components #
find_package(Boost 1.50.0 REQUIRED COMPONENTS filesystem system)

# Finding BFD libraries and headers #
find_path(BFD_INCLUDE_DIR bfd.h REQUIRED)
message(STATUS "Found BFD header: ${BFD_INCLUDE_DIR}/bfd.h")
find_library(BFD_LIBRARY libbfd.a REQUIRED)
set(BFD_LIBRARIES ${BFD_LIBRARY})
set(BFD_INCLUDE_DIRS ${BFD_INCLUDE_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BFD
                                  DEFAULT_MSG
                                  BFD_LIBRARY
                                  BFD_INCLUDE_DIR)
#################################################################
include_directories(${Boost_INCLUDE_DIRS})
include_directories(include)
add_subdirectory(include)
add_subdirectory(bfp)

enable_testing()
set(CMAKE_BUILD_TYPE "Coverage")
include(CMAKE_INCLUDES/CodeCoverage.cmake)
add_subdirectory(tests)
