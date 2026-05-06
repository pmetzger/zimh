add_unit_test(unit-swtp6800-dc4
    LABEL swtp6800
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dc4.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/swtp6800/common
        ${PROJECT_SOURCE_DIR}/simulators/swtp6800/swtp6800
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

# This target links the real M6800 sim_instr(), so it cannot use
# add_unit_test(), which supplies generic simulator personality stubs.
simh_executable_template(zimh-unit-swtp6800-m6800
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_m6800.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/swtp6800/common
        ${PROJECT_SOURCE_DIR}/simulators/swtp6800/swtp6800
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

target_link_libraries(zimh-unit-swtp6800-m6800 PUBLIC ${unittest_cmocka_target})
add_test(NAME zimh-unit-swtp6800-m6800 COMMAND zimh-unit-swtp6800-m6800)
set_tests_properties(zimh-unit-swtp6800-m6800 PROPERTIES
    LABELS "zimh;unit;zimh-swtp6800")
set_property(GLOBAL APPEND PROPERTY SIMH_UNIT_TEST_TARGETS
    zimh-unit-swtp6800-m6800)
