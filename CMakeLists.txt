cmake_minimum_required(VERSION 2.8)
project(BinaryFileParser)

#################################################################
# Finding BOOST components #
find_package(Boost COMPONENTS filesystem REQUIRED)

# Finding BFD libraries and headers #
find_path(BFD_INCLUDE_DIR bfd.h)
find_library(BFD_LIBRARY libbfd.a)
set(BFD_LIBRARIES ${BFD_LIBRARY})
set(BFD_INCLUDE_DIRS ${BFD_INCLUDE_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BFD
                                  DEFAULT_MSG
                                  BFD_LIBRARY
                                  BFD_INCLUDE_DIR)
mark_as_advanced(BFD_INCLUDE_DIR BFD_LIBRARY)

#################################################################
include_directories(${Boost_INCLUDE_DIRS})
include_directories(include)
add_subdirectory(include)
add_subdirectory(bfp)

enable_testing()

add_subdirectory(tests)
