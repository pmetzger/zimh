add_unit_test(unit-3b2-scsi
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_3b2_scsi.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
        ${PROJECT_SOURCE_DIR}/simulators/3B2
)

add_unit_test(unit-3b2-mau
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_3b2_mau.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
        ${PROJECT_SOURCE_DIR}/simulators/3B2
)
