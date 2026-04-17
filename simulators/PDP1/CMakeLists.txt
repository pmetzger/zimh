## PDP1 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(pdp1
    SOURCES
        pdp1_lp.c
        pdp1_cpu.c
        pdp1_stddev.c
        pdp1_sys.c
        pdp1_dt.c
        pdp1_drm.c
        pdp1_clk.c
        pdp1_dcs.c
        pdp1_dpy.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    DEFINES
        DISPLAY_TYPE=DIS_TYPE30
        PIX_SCALE=RES_HALF
    FEATURE_VIDEO
    FEATURE_DISPLAY
    LABEL PDP1
    PKG_FAMILY decpdp_family
    TEST pdp1)
