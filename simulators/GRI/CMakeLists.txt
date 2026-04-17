## GRI simulator
##
## Update this file as the simulator's sources and local build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this simulator later gets a local unit-test subtree, declare it
## explicitly here with add_subdirectory(unit-tests).

add_simulator(gri
    SOURCES
        gri_cpu.c
        gri_stddev.c
        gri_sys.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL GRI
    PKG_FAMILY grisys_family
    TEST gri)
