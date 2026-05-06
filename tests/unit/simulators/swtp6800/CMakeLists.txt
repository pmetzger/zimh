add_unit_test(unit-swtp6800-dc4
    LABEL swtp6800
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dc4.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/swtp6800/common
        ${PROJECT_SOURCE_DIR}/simulators/swtp6800/swtp6800
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)
