add_unit_test(unit-intel-systems-iram8
    SOURCES
        ${PROJECT_SOURCE_DIR}/simulators/Intel-Systems/common/iram8.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test_iram8.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/Intel-Systems/common
        ${PROJECT_SOURCE_DIR}/simulators/Intel-Systems/Intel-MDS
        ${PROJECT_SOURCE_DIR}/src/runtime
        ${PROJECT_SOURCE_DIR}/tests/unit/support
    LABEL intel-systems)
