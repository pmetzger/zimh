## imlac simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(imlac
    SOURCES
        imlac_sys.c
        imlac_cpu.c
        imlac_dp.c
        imlac_crt.c
        imlac_kbd.c
        imlac_tty.c
        imlac_pt.c
        imlac_bel.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    FEATURE_VIDEO
    FEATURE_DISPLAY
    USES_AIO
    LABEL imlac
    PKG_FAMILY imlac_family
    TEST imlac)
