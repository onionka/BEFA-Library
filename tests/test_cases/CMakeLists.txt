MACRO(SUBDIRLIST result curdir)
    FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
    SET(dirlist "")
    FOREACH(child ${children})
        IF(IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${child})
        ENDIF()
    ENDFOREACH()
    SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(SUBDIRS ${PROJECT_SOURCE_DIR}/tests/test_cases/)

SET(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -masm=intel)

FOREACH(subdir ${SUBDIRS})
    ADD_SUBDIRECTORY(${subdir})
    ADD_DEPENDENCIES(run_tests ${subdir})
ENDFOREACH()
