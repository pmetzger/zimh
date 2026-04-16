add_subdirectory(support)

add_unit_test(unit-smoke
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_smoke.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/support
)

add_unit_test(unit-support
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_support.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/support
)

add_unit_test(unit-sim-card
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_sim_card.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/support
)

add_unit_test(unit-sim-imd
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_sim_imd.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/support
)

add_unit_test(unit-scp-size
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_scp_size.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/support
)

add_unit_test(unit-scp-parse
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_scp_parse.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/support
)

add_unit_test(unit-scp-unit
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_scp_unit.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/support
)

add_unit_test(unit-scp-context
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_scp_context.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/support
)
