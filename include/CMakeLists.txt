add_subdirectory(bfp)

FILE (GLOB include_headers *.h *.hpp)
INSTALL(FILES ${include_headers}
        DESTINATION include)