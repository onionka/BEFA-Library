include_directories(bfp)

FILE (GLOB include_headers *.h *.hpp)
INSTALL(FILES ${include_headers}
        DESTINATION include)