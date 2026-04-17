## sigma simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(sigma
    SOURCES
        sigma_cpu.c
        sigma_sys.c
        sigma_cis.c
        sigma_coc.c
        sigma_dk.c
        sigma_dp.c
        sigma_fp.c
        sigma_io.c
        sigma_lp.c
        sigma_map.c
        sigma_mt.c
        sigma_pt.c
        sigma_rad.c
        sigma_rtc.c
        sigma_tt.c
        sigma_cr.c
        sigma_cp.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL sigma
    PKG_FAMILY sds_family
    TEST sigma)
