add_unit_test(unit-compat-time
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_compat_time.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)
