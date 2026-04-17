## I1401 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(i1401
    SOURCES
        i1401_lp.c
        i1401_cpu.c
        i1401_iq.c
        i1401_cd.c
        i1401_mt.c
        i1401_dp.c
        i1401_sys.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL I1401
    PKG_FAMILY ibm_family
    TEST i1401)
