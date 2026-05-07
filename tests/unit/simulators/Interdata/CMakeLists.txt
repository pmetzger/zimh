add_unit_test(unit-interdata-idc
    LABEL Interdata
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_id_idc.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/Interdata
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)
