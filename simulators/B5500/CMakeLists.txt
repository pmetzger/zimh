## B5500 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(b5500
    SOURCES
        b5500_cpu.c
        b5500_io.c
        b5500_sys.c
        b5500_dk.c
        b5500_mt.c
        b5500_urec.c
        b5500_dr.c
        b5500_dtc.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    DEFINES
        B5500
    FEATURE_INT64
    LABEL B5500
    PKG_FAMILY b5500_family
    TEST b5500)
