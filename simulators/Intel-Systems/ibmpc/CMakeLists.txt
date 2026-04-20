## Intel-Systems/ibmpc simulator
##
## Update this file as the local simulator sources and build metadata
## change. Keep shared target-construction logic in the common CMake helpers.
##
## This target is configured for explicit builds only. It remains outside the
## default simulator build until the repair work in projects/IBMPC.md is done.

simh_executable_template(ibmpc
    SOURCES
        ${INTELSYSC}/i8255.c
        ibmpc.c
        ${INTELSYSC}/i8088.c
        ibmpc_sys.c
        ${INTELSYSC}/i8253.c
        ${INTELSYSC}/i8259.c
        ${INTELSYSC}/pceprom.c
        ${INTELSYSC}/pcram8.c
        ${INTELSYSC}/i8237.c
        ${INTELSYSC}/pcbus.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(ibmpc PRIVATE ${SIMH_CORE_ROOT}/main.c)

if (DONT_USE_ROMS)
    target_compile_definitions(ibmpc PRIVATE DONT_USE_INTERNAL_ROM)
else ()
    add_dependencies(ibmpc BuildROMs)
endif ()
