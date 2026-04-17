## tt2500 simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(tt2500
    SOURCES
        tt2500_sys.c
        tt2500_cpu.c
        tt2500_dpy.c
        tt2500_crt.c
        tt2500_tv.c
        tt2500_key.c
        tt2500_uart.c
        tt2500_rom.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    FEATURE_VIDEO
    FEATURE_DISPLAY
    USES_AIO
    LABEL tt2500
    PKG_FAMILY default_family
    TEST tt2500)
