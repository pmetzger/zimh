## SDS simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(sds
    SOURCES
        sds_cpu.c
        sds_drm.c
        sds_dsk.c
        sds_io.c
        sds_lp.c
        sds_mt.c
        sds_mux.c
        sds_rad.c
        sds_stddev.c
        sds_sys.c
        sds_cp.c
        sds_cr.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL SDS
    PKG_FAMILY sds_family
    TEST sds)
