## TX-0 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(tx-0
    SOURCES
        tx0_cpu.c
        tx0_dpy.c
        tx0_stddev.c
        tx0_sys.c
        tx0_sys_orig.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    FEATURE_VIDEO
    FEATURE_DISPLAY
    LABEL TX-0
    PKG_FAMILY default_family
    TEST tx-0)
