add_unit_test(unit-linc-tape
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_linc_tape.c
        ${PROJECT_SOURCE_DIR}/simulators/linc/linc_tape.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/linc
)
