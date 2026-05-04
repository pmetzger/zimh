add_unit_test(unit-ibm1130-cr
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_ibm1130_cr.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/Ibm1130
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)
