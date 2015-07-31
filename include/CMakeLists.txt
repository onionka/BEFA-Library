add_subdirectory(bfp)

FILE (GLOB include_headers *.h *.hpp *.tpp)
INSTALL(FILES ${include_headers}
        DESTINATION include)