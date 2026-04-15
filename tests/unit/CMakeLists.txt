add_subdirectory(support)

add_unit_test(unit-smoke
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_smoke.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/support
)
