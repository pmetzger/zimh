## I7094 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(i7094
    SOURCES
        i7094_cpu.c
        i7094_cpu1.c
        i7094_io.c
        i7094_cd.c
        i7094_clk.c
        i7094_com.c
        i7094_drm.c
        i7094_dsk.c
        i7094_sys.c
        i7094_lp.c
        i7094_mt.c
        i7094_binloader.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    FEATURE_INT64
    LABEL I7094
    PKG_FAMILY ibm_family
    TEST i7094)
