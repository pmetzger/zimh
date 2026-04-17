## ND100 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(nd100
    SOURCES
        nd100_sys.c
        nd100_cpu.c
        nd100_floppy.c
        nd100_stddev.c
        nd100_mm.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL ND100
    PKG_FAMILY norsk_family
    TEST nd100)
