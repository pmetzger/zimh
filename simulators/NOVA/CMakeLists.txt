## NOVA simulators
##
## Update this file as the simulators' sources and local build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if these simulators later get a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(eclipse
    SOURCES
        eclipse_cpu.c
        eclipse_tt.c
        nova_sys.c
        nova_dkp.c
        nova_dsk.c
        nova_lp.c
        nova_mta.c
        nova_plt.c
        nova_pt.c
        nova_clk.c
        nova_tt1.c
        nova_qty.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    DEFINES
        ECLIPSE
    FEATURE_INT64
    LABEL NOVA
    PKG_FAMILY dgnova_family
    TEST eclipse)

add_simulator(nova
    SOURCES
        nova_sys.c
        nova_cpu.c
        nova_dkp.c
        nova_dsk.c
        nova_lp.c
        nova_mta.c
        nova_plt.c
        nova_pt.c
        nova_clk.c
        nova_tt.c
        nova_tt1.c
        nova_qty.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    LABEL NOVA
    PKG_FAMILY dgnova_family
    TEST nova)
