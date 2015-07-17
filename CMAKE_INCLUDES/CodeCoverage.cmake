if ("${USE_COMPILER}" STREQUAL "clang")
	FIND_PROGRAM(GCOV_PATH llvm-cov)
else()
	FIND_PROGRAM(GCOV_PATH gcov)
endif()
FIND_PROGRAM(LCOV_PATH lcov)
FIND_PROGRAM(GCOVR_PATH gcovr PATHS ${CMAKE_SOURCE_DIR}/tests)

IF(NOT GCOV_PATH)
	MESSAGE(FATAL_ERROR "gcov not found! Aborting...")
ENDIF()

IF(NOT CMAKE_COMPILER_IS_GNUCXX)

	MESSAGE(WARNING "Compiler is not GNU gcc! Clang Version 3.0.0 and greater supports gcov as well, but older versions don't.")

	IF(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		MESSAGE(FATAL_ERROR "Compiler is not GNU gcc! Aborting...")
	ENDIF()

ENDIF()

SET(CMAKE_CXX_FLAGS_COVERAGE
    "-g -O0 -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C++ compiler during coverage builds."
    FORCE)

SET(CMAKE_C_FLAGS_COVERAGE
    "-g -O0 -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C compiler during coverage builds."
    FORCE)

if ("${USE_COMPILER}" STREQUAL "clang")
	SET(CMAKE_CXX_FLAGS_COVERAGE
		"${CMAKE_CXX_FLAGS_COVERAGE} -Xclang -coverage-cfg-checksum -Xclang -coverage-no-function-names-in-data -Xclang -coverage-version='407*'")
	SET(CMAKE_C_FLAGS_COVERAGE
		"${CMAKE_C_FLAGS_COVERAGE} -Xclang -coverage-cfg-checksum -Xclang -coverage-no-function-names-in-data -Xclang -coverage-version='407*'")
endif ()


SET(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    "--coverage"
    CACHE STRING "Flags used for linking binaries during coverage builds."
    FORCE)

SET(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    ""
    CACHE STRING "Flags used by the shared libraries linker during coverage builds."
    FORCE)

MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE)

IF (NOT (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Coverage"))
  MESSAGE(WARNING "Code coverage results with an optimized (non-Debug) build may be misleading")
ENDIF()


# Param _targetname     The name of new the custom make target
# Param _testrunner     The name of the target which runs the tests.
#						MUST return ZERO always, even on errors.
#						If not, no coverage report will be created!
# Param _outputname     lcov output is generated as _outputname.info
#                       HTML report is generated in _outputname/index.html
FUNCTION(SETUP_TARGET_FOR_COVERAGE _targetname _testrunner _outputname)

	IF(NOT LCOV_PATH)
		MESSAGE(FATAL_ERROR "lcov not found! Aborting...")
	ENDIF()

	ADD_CUSTOM_TARGET(${_targetname}
		COMMAND ${LCOV_PATH} --directory . --zerocounters
		COMMAND ${_testrunner}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Cleaning up lcov and running functional tests")

	ADD_CUSTOM_COMMAND(TARGET ${_targetname} POST_BUILD
		COMMAND ${LCOV_PATH} --directory . --capture --output-file ${_outputname}.info
		COMMAND ${LCOV_PATH} --remove coverage.info 'tests/*' '/usr/*'  '*/exception.hpp' '*/exception.cpp' --output-file ${_outputname}.info
		COMMAND ${LCOV_PATH} --list ${_outputname}.info
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Processing code coverage counters and generating report.")

ENDFUNCTION()
