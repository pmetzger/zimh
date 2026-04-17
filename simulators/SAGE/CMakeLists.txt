## SAGE simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(sage
    SOURCES
        sage_cpu.c
        sage_sys.c
        sage_stddev.c
        sage_cons.c
        sage_fd.c
        sage_lp.c
        m68k_cpu.c
        m68k_mem.c
        m68k_scp.c
        m68k_parse.tab.c
        m68k_sys.c
        i8251.c
        i8253.c
        i8255.c
        i8259.c
        i8272.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    DEFINES
        HAVE_INT64
    LABEL SAGE
    PKG_FAMILY experimental
    TEST sage)
