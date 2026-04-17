## S3 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(s3
    SOURCES
        s3_cd.c
        s3_cpu.c
        s3_disk.c
        s3_lp.c
        s3_pkb.c
        s3_sys.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL S3
    PKG_FAMILY ibm_family
    TEST s3)
