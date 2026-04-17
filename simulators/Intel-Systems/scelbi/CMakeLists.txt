## Intel-Systems/scelbi simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(scelbi
    SOURCES
        ${SCELBIC}/i8008.c
        scelbi_sys.c
        scelbi_io.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL Intel-Systems
    PKG_FAMILY intel_family
    TEST scelbi)
