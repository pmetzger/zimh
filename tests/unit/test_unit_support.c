#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <cmocka.h>

#include "test_scp_fixture.h"
#include "test_support.h"
#include "test_simh_personality.h"

/* Verify that fixture lookup and fixture reads use the expected tree. */
static void test_support_reads_fixture(void **state)
{
    static const char fixture_text[] = "sample fixture for unit tests\n";
    char path[1024];
    void *data;
    size_t size;

    (void)state;

    assert_int_equal(simh_test_fixture_path(path, sizeof(path), "sample.txt"),
                     0);
    assert_true(strstr(path, "tests/unit/fixtures/sample.txt") != NULL);

    assert_int_equal(simh_test_read_fixture("sample.txt", &data, &size), 0);
    assert_int_equal(size, strlen(fixture_text));
    assert_string_equal((const char *)data, fixture_text);

    free(data);
}

/* Verify that path joining produces the expected simple path. */
static void test_support_joins_paths(void **state)
{
    char path[1024];

    (void)state;

    assert_int_equal(
        simh_test_join_path(path, sizeof(path), "/tmp", "file.txt"), 0);
    assert_string_equal(path, "/tmp/file.txt");
}

/* Verify temp-directory, write, compare, and recursive cleanup helpers. */
static void test_support_temp_dir_round_trip(void **state)
{
    char dir_path[1024];
    char fixture_path[1024];
    char output_path[1024];
    void *data;
    size_t size;
    struct stat st;
    int length;

    (void)state;

    assert_int_equal(
        simh_test_make_temp_dir(dir_path, sizeof(dir_path), "simh-unit"), 0);
    assert_int_equal(stat(dir_path, &st), 0);
    assert_true(S_ISDIR(st.st_mode));

    assert_int_equal(simh_test_fixture_path(fixture_path, sizeof(fixture_path),
                                            "sample.txt"),
                     0);
    assert_int_equal(simh_test_read_fixture("sample.txt", &data, &size), 0);

    length = snprintf(output_path, sizeof(output_path), "%s/%s", dir_path,
                      "copy.txt");
    assert_true(length > 0);
    assert_true((size_t)length < sizeof(output_path));
    assert_int_equal(simh_test_write_file(output_path, data, size), 0);
    assert_int_equal(simh_test_files_equal(fixture_path, output_path), 1);

    free(data);

    assert_int_equal(simh_test_remove_path(dir_path), 0);
    assert_int_not_equal(stat(dir_path, &st), 0);
}

/* Verify minimal one-unit device initialization for later common tests. */
static void test_support_initializes_device_and_unit(void **state)
{
    DEVICE device;
    UNIT unit;

    (void)state;

    simh_test_init_device_unit(&device, &unit, "DEV", "DEV0", DEV_CARD,
                               UNIT_ATTABLE | UNIT_ROABLE, 16, 2);

    assert_string_equal(device.name, "DEV");
    assert_ptr_equal(device.units, &unit);
    assert_int_equal(device.numunits, 1);
    assert_int_equal(device.flags, DEV_CARD);
    assert_int_equal(device.dwidth, 16);
    assert_int_equal(device.aincr, 2);
    assert_true((unit.flags & UNIT_ATTABLE) != 0);
    assert_true((unit.flags & UNIT_ROABLE) != 0);
    assert_string_equal(unit.uname, "DEV0");
    assert_ptr_equal(unit.dptr, &device);
}

/* Verify SCP fixture helpers install devices and free cached unit names. */
static void test_scp_fixture_helpers_install_devices(void **state)
{
    DEVICE device;
    UNIT units[2];
    DEVICE *devices[2];

    (void)state;

    simh_test_init_multiunit_device(&device, units, 2, "DSK", "SYSDISK", 0);
    devices[0] = &device;
    devices[1] = NULL;

    assert_int_equal(
        simh_test_install_devices("simh-unit-test-support", devices), 0);
    assert_ptr_equal(sim_devices[0], &device);
    assert_string_equal(sim_name, "simh-unit-test-support");

    assert_string_equal(sim_set_uname(&units[0], "SYSDISK0"), "SYSDISK0");
    assert_string_equal(sim_set_uname(&units[1], "SYSDISK1"), "SYSDISK1");

    simh_test_free_unit_names(units, 2);
    assert_null(units[0].uname);
    assert_null(units[1].uname);

    simh_test_reset_simulator_state();
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_support_reads_fixture),
        cmocka_unit_test(test_support_joins_paths),
        cmocka_unit_test(test_support_temp_dir_round_trip),
        cmocka_unit_test(test_support_initializes_device_and_unit),
        cmocka_unit_test(test_scp_fixture_helpers_install_devices),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
