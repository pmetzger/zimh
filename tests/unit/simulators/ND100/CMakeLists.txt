add_unit_test(unit-nd100-floppy
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_nd100_floppy.c
        ${PROJECT_SOURCE_DIR}/simulators/ND100/nd100_floppy.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/ND100
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

simh_executable_template(zimh-unit-nd100-cpu-shift
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_nd100_cpu_shift.c
        ${PROJECT_SOURCE_DIR}/simulators/ND100/nd100_cpu.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/ND100
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

target_link_libraries(zimh-unit-nd100-cpu-shift PUBLIC ${unittest_cmocka_target})
add_test(NAME zimh-unit-nd100-cpu-shift COMMAND zimh-unit-nd100-cpu-shift)
set_tests_properties(zimh-unit-nd100-cpu-shift PROPERTIES
    LABELS "zimh;unit;zimh-unit")
set_property(GLOBAL APPEND PROPERTY SIMH_UNIT_TEST_TARGETS
    zimh-unit-nd100-cpu-shift)
