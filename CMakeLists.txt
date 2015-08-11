CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(BinaryFileParser)
IF ("${USE_COMPILER}" STREQUAL "clang")
    SET(CMAKE_C_COMPILER /usr/bin/clang)
    SET(CMAKE_CXX_COMPILER /usr/bin/clang++)
ENDIF ()
#################################################################
# Finding BOOST components #
FIND_PACKAGE(Boost 1.50.0 REQUIRED COMPONENTS unit_test_framework REQUIRED)
# Finding BFD libraries and headers #
FIND_LIBRARY(BFD_LIBRARY libbfd.a REQUIRED)
SET(BFD_LIBRARIES ${BFD_LIBRARY})
#################################################################
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(include)
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(bfp)
#################################################################
FIND_FILE(DOXYGEN_HTML_CONF dox_configs/config_html.dox HINT ${PROJECT_SOURCE_DIR})
FIND_FILE(DOXYGEN_MAN_CONF dox_configs/config_man.dox HINT ${PROJECT_SOURCE_DIR})
FIND_PROGRAM(DOXYGEN doxygen)
ADD_CUSTOM_TARGET(doc-html
  COMMAND ${DOXYGEN} ${DOXYGEN_HTML_CONF}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ADD_CUSTOM_TARGET(doc-man
  COMMAND ${DOXYGEN} ${DOXYGEN_MAN_CONF}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ADD_SUBDIRECTORY(doc)
#################################################################
ENABLE_TESTING()
FIND_PROGRAM(VAL valgrind)
SET(VALGRIND ${VAL} --leak-check=full --show-reachable=yes)
INCLUDE(CMAKE_INCLUDES/CodeCoverage.cmake)
ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(examples)