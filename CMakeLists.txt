CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(BinaryFileParser)
#################################################################
# Finding BOOST components #
FIND_PACKAGE(Boost 1.50.0 REQUIRED COMPONENTS unit_test_framework program_options REQUIRED)
# Finding BFD libraries and headers #
FIND_LIBRARY(BFD_LIBRARY libbfd.a REQUIRED)
SET(BFD_LIBRARIES ${BFD_LIBRARY})
#################################################################
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(include)
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(bfp)
#################################################################
SET(DOXYGEN_CONFIG_PATH ${PROJECT_SOURCE_DIR}/dox_configs)
ADD_SUBDIRECTORY(dox_configs)
ADD_SUBDIRECTORY(doc)
#################################################################
ENABLE_TESTING()
FIND_PROGRAM(VAL valgrind)
SET(VALGRIND ${VAL} --leak-check=full --show-reachable=yes)
IF (NOT EXISTS ${VAL})
  MESSAGE(WARNING "Valgrind is not available! running tests without it ...")
  SET(VALGRIND)
ENDIF()
IF (CMAKE_BUILD_TYPE STREQUAL "Coverage")
  INCLUDE(CMAKE_INCLUDES/CodeCoverage.cmake)
ENDIF ()
ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(examples)