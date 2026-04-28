add_unit_test(unit-sim-fio
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_fio.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-sim-dynstr
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_dynstr.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

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

add_unit_test(unit-sim-tape
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_tape.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-sim-disk
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_disk.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-sim-timer
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_timer.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-sim-scsi
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_scsi.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-sim-time
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_time.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-sim-sock
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_sock.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-sim-ether
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_ether.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-sim-video
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_video.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)

add_unit_test(unit-sim-tmxr
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sim_tmxr.c
    INCLUDES
        ${PROJECT_SOURCE_DIR}/tests/unit/support
)
