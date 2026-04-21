## SAGE simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

find_program(BISON_COMMAND bison REQUIRED)

set(sage_parser_c "${CMAKE_CURRENT_BINARY_DIR}/m68k_parse.tab.c")
set(sage_parser_h "${CMAKE_CURRENT_BINARY_DIR}/m68k_parse.tab.h")

add_custom_command(
    OUTPUT
        "${sage_parser_c}"
    COMMAND "${BISON_COMMAND}"
        -d
        -o "${sage_parser_c}"
        "${CMAKE_CURRENT_SOURCE_DIR}/m68k_parse.y"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/m68k_parse.y"
    BYPRODUCTS
        "${sage_parser_h}"
    VERBATIM
)

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
        ${sage_parser_c}
        m68k_sys.c
        i8251.c
        i8253.c
        i8255.c
        i8259.c
        i8272.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    DEFINES
        HAVE_INT64
    LABEL SAGE
    PKG_FAMILY experimental_family
    TEST sage)
