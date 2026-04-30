add_unit_test(unit-compat-string
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_compat_string.c
        ${SIMH_COMPAT_ROOT}/strlcpy.c
        ${SIMH_COMPAT_ROOT}/strlcat.c
    DEFINES
        SIMH_NEED_STRLCPY
        SIMH_NEED_STRLCAT
)

add_unit_test(unit-compat-temp
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_compat_temp.c
        ${SIMH_COMPAT_ROOT}/tempfile.c
    DEFINES
        SIMH_COMPAT_TEST
)

if (WIN32)
    add_unit_test(unit-compat-time
        LABEL unit
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/test_compat_time.c
    )

    add_unit_test(unit-compat-env
        LABEL unit
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/test_compat_env.c
    )
endif ()
