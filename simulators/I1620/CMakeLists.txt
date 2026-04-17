## I1620 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(i1620
    SOURCES
        i1620_cd.c
        i1620_dp.c
        i1620_pt.c
        i1620_tty.c
        i1620_cpu.c
        i1620_lp.c
        i1620_fp.c
        i1620_sys.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL I1620
    PKG_FAMILY ibm_family
    TEST i1620)
