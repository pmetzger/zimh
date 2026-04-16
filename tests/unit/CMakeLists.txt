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

add_executable(simbase-unit-sim-card
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_sim_card.c
    ${SIMH_RUNTIME_ROOT}/sim_card.c
)

set_target_properties(simbase-unit-sim-card PROPERTIES
    C_STANDARD 99
)

target_compile_definitions(simbase-unit-sim-card PRIVATE
    USE_SIM_CARD
)

target_include_directories(simbase-unit-sim-card PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/support
    ${SIMH_CORE_ROOT}
    ${SIMH_RUNTIME_ROOT}
)

target_link_libraries(simbase-unit-sim-card PRIVATE
    unittest
    os_features
)

add_test(NAME simbase-unit-sim-card COMMAND simbase-unit-sim-card)
set_tests_properties(simbase-unit-sim-card PROPERTIES
    LABELS "simbase;unit;simbase-unit"
)
