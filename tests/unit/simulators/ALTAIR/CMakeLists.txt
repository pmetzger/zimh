add_unit_test(unit-altair-dsk
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_altair_dsk.c
        ${PROJECT_SOURCE_DIR}/simulators/ALTAIR/altair_dsk.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/ALTAIR
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)
