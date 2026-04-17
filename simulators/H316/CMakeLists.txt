## H316 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(h316
    SOURCES
        h316_stddev.c
        h316_lp.c
        h316_cpu.c
        h316_sys.c
        h316_mt.c
        h316_fhd.c
        h316_dp.c
        h316_rtc.c
        h316_imp.c
        h316_hi.c
        h316_mi.c
        h316_udp.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    DEFINES
        VM_IMPTIP
    LABEL H316
    PKG_FAMILY honeywell_family
    TEST h316)
