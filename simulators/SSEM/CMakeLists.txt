## SSEM simulator
##
## Update this file as the simulator's sources and local build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this simulator later gets a local unit-test subtree, declare it
## explicitly here with add_subdirectory(unit-tests).

add_simulator(ssem
    SOURCES
        ssem_cpu.c
        ssem_sys.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL SSEM
    PKG_FAMILY default_family
    TEST ssem)
