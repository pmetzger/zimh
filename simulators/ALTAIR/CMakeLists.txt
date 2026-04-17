## ALTAIR simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(altair
    SOURCES
        altair_sio.c
        altair_cpu.c
        altair_dsk.c
        altair_sys.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL ALTAIR
    PKG_FAMILY default_family
    TEST altair)
