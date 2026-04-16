add_unit_test(unit-sim-card
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_card.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-sim-imd
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_imd.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)
