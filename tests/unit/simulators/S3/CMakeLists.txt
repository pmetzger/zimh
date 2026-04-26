add_unit_test(unit-s3-cd
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_s3_cd.c
        ${PROJECT_SOURCE_DIR}/simulators/S3/s3_cd.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/simulators/S3
)
