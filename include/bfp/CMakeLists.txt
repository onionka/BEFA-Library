FILE(GLOB bfp_headers *.hpp *.h)

INSTALL(FILES ${bfp_headers}
        DESTINATION include/bfp)
