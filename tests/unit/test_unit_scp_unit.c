#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "scp.h"
#include "test_simh_personality.h"
#include "test_support.h"

struct scp_unit_fixture {
    char temp_dir[1024];
    char file_path[1024];
    UNIT unit;
    DEVICE device;
};

static int setup_scp_unit_fixture(void **state)
{
    struct scp_unit_fixture *fixture;
    DEVICE *devices[2];

    fixture = calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    assert_int_equal(simh_test_make_temp_dir(fixture->temp_dir,
                                             sizeof(fixture->temp_dir),
                                             "scp-unit"),
                     0);
    assert_int_equal(simh_test_join_path(fixture->file_path,
                                         sizeof(fixture->file_path),
                                         fixture->temp_dir, "unit.bin"),
                     0);

    simh_test_init_device_unit(&fixture->device, &fixture->unit, "DSK", "DSK0",
                               0, UNIT_ATTABLE, 8, 1);

    simh_test_reset_simulator_state();
    assert_int_equal(simh_test_set_sim_name("simbase-unit-scp-unit"), 0);
    devices[0] = &fixture->device;
    devices[1] = NULL;
    assert_int_equal(simh_test_set_devices(devices), 0);

    sim_switches = 0;
    sim_switch_number = 0;

    *state = fixture;
    return 0;
}

static int teardown_scp_unit_fixture(void **state)
{
    struct scp_unit_fixture *fixture = *state;

    if ((fixture->unit.flags & UNIT_ATT) != 0)
        assert_int_equal(detach_unit(&fixture->unit), SCPE_OK);

    assert_int_equal(simh_test_remove_path(fixture->temp_dir), 0);
    free(fixture);
    *state = NULL;
    return 0;
}

static void test_find_dev_from_unit_locates_simulator_device(void **state)
{
    struct scp_unit_fixture *fixture = *state;

    fixture->unit.dptr = NULL;

    assert_ptr_equal(find_dev_from_unit(&fixture->unit), &fixture->device);
    assert_ptr_equal(fixture->unit.dptr, &fixture->device);
}

static void test_attach_unit_creates_new_file_and_detaches_cleanly(void **state)
{
    struct scp_unit_fixture *fixture = *state;
    void *file_data;
    size_t file_size;

    sim_switches = SWMASK('N');

    assert_int_equal(attach_unit(&fixture->unit, fixture->file_path), SCPE_OK);
    assert_true((fixture->unit.flags & UNIT_ATT) != 0);
    assert_non_null(fixture->unit.fileref);
    assert_non_null(fixture->unit.filename);

    assert_int_equal(detach_unit(&fixture->unit), SCPE_OK);
    assert_true((fixture->unit.flags & UNIT_ATT) == 0);
    assert_null(fixture->unit.fileref);
    assert_null(fixture->unit.filename);

    assert_int_equal(
        simh_test_read_file(fixture->file_path, &file_data, &file_size), 0);
    assert_int_equal(file_size, 0);
    free(file_data);
}

static void test_attach_unit_honors_read_only_mode(void **state)
{
    static const uint8_t initial_data[] = {0x11, 0x22, 0x33, 0x44};
    struct scp_unit_fixture *fixture = *state;

    fixture->unit.flags = UNIT_ATTABLE | UNIT_ROABLE;
    assert_int_equal(simh_test_write_file(fixture->file_path, initial_data,
                                          sizeof(initial_data)),
                     0);

    sim_switches = SWMASK('R');

    assert_int_equal(attach_unit(&fixture->unit, fixture->file_path), SCPE_OK);
    assert_true((fixture->unit.flags & UNIT_RO) != 0);

    assert_int_equal(detach_unit(&fixture->unit), SCPE_OK);
    assert_true((fixture->unit.flags & UNIT_RO) == 0);
}

static void test_detach_unit_flushes_buffered_changes(void **state)
{
    static const uint8_t initial_data[] = {'A', 'B', 'C', 'D'};
    static const uint8_t expected_data[] = {'Z', 'B', 'C', 'D'};
    struct scp_unit_fixture *fixture = *state;
    void *file_data;
    size_t file_size;

    fixture->unit.flags = UNIT_ATTABLE | UNIT_BUFABLE | UNIT_MUSTBUF;
    fixture->unit.capac = sizeof(initial_data);
    fixture->device.dwidth = 8;
    fixture->device.aincr = 1;

    assert_int_equal(simh_test_write_file(fixture->file_path, initial_data,
                                          sizeof(initial_data)),
                     0);

    assert_int_equal(attach_unit(&fixture->unit, fixture->file_path), SCPE_OK);
    assert_non_null(fixture->unit.filebuf);
    assert_non_null(fixture->unit.filebuf2);

    ((uint8_t *)fixture->unit.filebuf)[0] = 'Z';

    assert_int_equal(detach_unit(&fixture->unit), SCPE_OK);
    assert_null(fixture->unit.filebuf);
    assert_null(fixture->unit.filebuf2);

    assert_int_equal(
        simh_test_read_file(fixture->file_path, &file_data, &file_size), 0);
    assert_int_equal(file_size, sizeof(expected_data));
    assert_memory_equal(file_data, expected_data, sizeof(expected_data));
    free(file_data);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_find_dev_from_unit_locates_simulator_device,
            setup_scp_unit_fixture, teardown_scp_unit_fixture),
        cmocka_unit_test_setup_teardown(
            test_attach_unit_creates_new_file_and_detaches_cleanly,
            setup_scp_unit_fixture, teardown_scp_unit_fixture),
        cmocka_unit_test_setup_teardown(test_attach_unit_honors_read_only_mode,
                                        setup_scp_unit_fixture,
                                        teardown_scp_unit_fixture),
        cmocka_unit_test_setup_teardown(
            test_detach_unit_flushes_buffered_changes, setup_scp_unit_fixture,
            teardown_scp_unit_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
