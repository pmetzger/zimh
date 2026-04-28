add_unit_test(unit-vax4xx-dz
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_vax4xx_dz.c
        ${PROJECT_SOURCE_DIR}/simulators/VAX/vax4xx_dz.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/VAX
    DEFINES
        VM_VAX
        VAX_440
        VAX_48
    FEATURE_FULL64
)

add_unit_test(unit-vax4xx-rz94
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_vax4xx_rz94.c
        ${PROJECT_SOURCE_DIR}/simulators/VAX/vax4xx_rz94.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/VAX
    DEFINES
        VM_VAX
        VAX_440
        VAX_48
    FEATURE_FULL64
)
