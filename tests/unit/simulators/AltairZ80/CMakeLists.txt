add_unit_test(unit-altairz80-dazzler
    LABEL AltairZ80
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_s100_dazzler.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/AltairZ80
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)
