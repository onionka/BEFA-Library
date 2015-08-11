IF (WITH_MAN)
  FILE(GLOB mans PATH ${PROJECT_SOURCE_DIR}/doc/man/man3/*.3)
	INSTALL(FILES ${mans}
		      DESTINATION share/man/man3)
ENDIF ()