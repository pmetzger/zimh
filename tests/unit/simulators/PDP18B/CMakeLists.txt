add_unit_test(unit-pdp18b-cpu
    LABEL PDP18B
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_pdp18b_cpu.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/PDP18B
        ${PROJECT_SOURCE_DIR}/tests/unit/support
    DEFINES
        PDP9
)
