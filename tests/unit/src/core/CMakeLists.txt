add_unit_test(unit-scp-size
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_scp_size.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-scp-parse
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_scp_parse.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-scp-unit
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_scp_unit.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-scp-context
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_scp_context.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)
