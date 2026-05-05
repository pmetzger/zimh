add_unit_test(unit-besm6-arith
    LABEL BESM6
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_besm6_arith.c
        ${PROJECT_SOURCE_DIR}/simulators/BESM6/besm6_arith.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/BESM6
        ${PROJECT_SOURCE_DIR}/tests/unit/support
    FEATURE_INT64
)
