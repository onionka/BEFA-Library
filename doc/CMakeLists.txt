IF (WITH_MAN)
  FILE(GLOB mans PATH ${PROJECT_SOURCE_DIR}/doc/man/man3/*.3)
  FOREACH (man ${mans})
    INSTALL(FILES ${man}
            DESTINATION share/man/man3)
  ENDFOREACH ()
ENDIF ()