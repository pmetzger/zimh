## CDC1700 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(cdc1700
    SOURCES
        cdc1700_cpu.c
        cdc1700_dis.c
        cdc1700_io.c
        cdc1700_sys.c
        cdc1700_dev1.c
        cdc1700_mt.c
        cdc1700_dc.c
        cdc1700_iofw.c
        cdc1700_lp.c
        cdc1700_dp.c
        cdc1700_cd.c
        cdc1700_sym.c
        cdc1700_rtc.c
        cdc1700_drm.c
        cdc1700_msos5.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL CDC1700
    PKG_FAMILY cdc1700_family
    TEST cdc1700)
