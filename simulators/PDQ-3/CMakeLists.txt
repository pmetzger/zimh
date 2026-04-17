## PDQ-3 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(pdq3
    SOURCES
        pdq3_cpu.c
        pdq3_sys.c
        pdq3_stddev.c
        pdq3_mem.c
        pdq3_debug.c
        pdq3_fdc.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL PDQ-3
    PKG_FAMILY experimental
    TEST pdq3)
