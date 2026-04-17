## swtp6800/swtp6800 simulators
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(swtp6800mp-a
    SOURCES
        ${SWTP6800C}/mp-a.c
        ${SWTP6800C}/m6800.c
        ${SWTP6800C}/m6810.c
        ${SWTP6800C}/bootrom.c
        ${SWTP6800C}/dc-4.c
        mp-a_sys.c
        ${SWTP6800C}/mp-8m.c
        ${SWTP6800C}/fd400.c
        ${SWTP6800C}/mp-b2.c
        ${SWTP6800C}/mp-s.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    BUILDROMS
    LABEL swtp6800
    PKG_FAMILY swtp_family
    TEST swtp6800mp-a)

add_simulator(swtp6800mp-a2
    SOURCES
        ${SWTP6800C}/mp-a2.c
        ${SWTP6800C}/m6800.c
        ${SWTP6800C}/m6810.c
        ${SWTP6800C}/bootrom.c
        ${SWTP6800C}/dc-4.c
        mp-a2_sys.c
        ${SWTP6800C}/mp-8m.c
        ${SWTP6800C}/i2716.c
        ${SWTP6800C}/fd400.c
        ${SWTP6800C}/mp-s.c
        ${SWTP6800C}/mp-b2.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    BUILDROMS
    LABEL swtp6800
    PKG_FAMILY swtp_family
    TEST swtp6800mp-a2)
