add_unit_test(unit-linc-tape
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_linc_tape.c
        ${PROJECT_SOURCE_DIR}/simulators/linc/linc_tape.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/linc
)

# This target links the real LINC sim_instr(), so it cannot use
# add_unit_test(), which supplies generic simulator personality stubs.
simh_executable_template(zimh-unit-linc-cpu
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_linc_cpu.c
        ${PROJECT_SOURCE_DIR}/simulators/linc/linc_cpu.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/linc
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

target_link_libraries(zimh-unit-linc-cpu PUBLIC ${unittest_cmocka_target})
add_test(NAME zimh-unit-linc-cpu COMMAND zimh-unit-linc-cpu)
set_tests_properties(zimh-unit-linc-cpu PROPERTIES LABELS "zimh;unit;zimh-unit")
set_property(GLOBAL APPEND PROPERTY SIMH_UNIT_TEST_TARGETS zimh-unit-linc-cpu)
