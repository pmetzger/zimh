add_unit_test(unit-nd100-floppy
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_nd100_floppy.c
        ${PROJECT_SOURCE_DIR}/simulators/ND100/nd100_floppy.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/ND100
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)
