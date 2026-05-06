add_unit_test(unit-pdp8-td
    LABEL PDP8
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_pdp8_td.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/PDP8
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)
