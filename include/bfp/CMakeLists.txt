FILE(GLOB bfp_headers *.hpp *.h *.tpp)

INSTALL(FILES ${bfp_headers}
        DESTINATION include/bfp)
