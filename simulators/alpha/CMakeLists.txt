## alpha simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## Not yet: if this directory later gets a local unit-test subtree, declare
## it explicitly here with add_subdirectory(unit-tests).

add_simulator(alpha
    SOURCES
        alpha_500au_syslist.c
        alpha_cpu.c
        alpha_ev5_cons.c
        alpha_ev5_pal.c
        alpha_ev5_tlb.c
        alpha_fpi.c
        alpha_fpv.c
        alpha_io.c
        alpha_mmu.c
        alpha_sys.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    FEATURE_FULL64
    LABEL alpha
    PKG_FAMILY experimental
    TEST alpha)
