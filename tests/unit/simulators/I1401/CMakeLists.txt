add_unit_test(unit-i1401-bool
    LABEL I1401
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_i1401_bool.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/I1401
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

simh_executable_template(zimh-unit-i1401-sys
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_i1401_sys.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/I1401
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

target_link_libraries(zimh-unit-i1401-sys PUBLIC ${unittest_cmocka_target})
add_test(NAME zimh-unit-i1401-sys COMMAND zimh-unit-i1401-sys)
set_tests_properties(zimh-unit-i1401-sys PROPERTIES
    LABELS "zimh;unit;zimh-I1401")
set_property(GLOBAL APPEND PROPERTY SIMH_UNIT_TEST_TARGETS
    zimh-unit-i1401-sys)
