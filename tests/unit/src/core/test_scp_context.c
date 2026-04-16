#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "scp.h"
#include "test_simh_personality.h"
#include "test_support.h"

struct scp_context_fixture {
    DEVICE disk;
    UNIT disk_units[2];
    DEVICE reader;
    UNIT reader_unit;
    DEVICE disabled;
    UNIT disabled_unit;
    DEVICE internal;
    UNIT internal_unit;
};

static void init_multiunit_device(DEVICE *device, UNIT *units, uint32 numunits,
                                  const char *name, const char *lname,
                                  uint32 flags)
{
    uint32 i;

    memset(device, 0, sizeof(*device));
    memset(units, 0, numunits * sizeof(*units));

    device->name = name;
    device->lname = (char *)lname;
    device->units = units;
    device->numunits = numunits;
    device->flags = flags;

    for (i = 0; i < numunits; ++i)
        units[i].dptr = device;
}

static void free_unit_name(UNIT *unit)
{
    free(unit->uname);
    unit->uname = NULL;
}

static int setup_scp_context_fixture(void **state)
{
    struct scp_context_fixture *fixture;
    DEVICE *devices[4];

    fixture = calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    init_multiunit_device(&fixture->disk, fixture->disk_units, 2, "DQ",
                          "SYSDISK", 0);
    simh_test_init_device_unit(&fixture->reader, &fixture->reader_unit, "PTR",
                               NULL, 0, 0, 8, 1);
    fixture->reader.lname = "READER";
    fixture->reader_unit.uname = NULL;

    simh_test_init_device_unit(&fixture->disabled, &fixture->disabled_unit,
                               "TTY", NULL, DEV_DIS, 0, 8, 1);
    fixture->disabled_unit.uname = NULL;

    simh_test_init_device_unit(&fixture->internal, &fixture->internal_unit,
                               "CLK", NULL, 0, 0, 8, 1);
    fixture->internal.lname = "CLOCK";
    fixture->internal_unit.uname = NULL;

    simh_test_reset_simulator_state();
    assert_int_equal(simh_test_set_sim_name("simbase-unit-scp-context"), 0);
    devices[0] = &fixture->disk;
    devices[1] = &fixture->reader;
    devices[2] = &fixture->disabled;
    devices[3] = NULL;
    assert_int_equal(simh_test_set_devices(devices), 0);

    *state = fixture;
    return 0;
}

static int teardown_scp_context_fixture(void **state)
{
    struct scp_context_fixture *fixture = *state;

    free_unit_name(&fixture->disk_units[0]);
    free_unit_name(&fixture->disk_units[1]);
    free_unit_name(&fixture->reader_unit);
    free_unit_name(&fixture->disabled_unit);
    free_unit_name(&fixture->internal_unit);
    simh_test_reset_simulator_state();
    free(fixture);
    *state = NULL;
    return 0;
}

/* Verify device display names prefer logical aliases over physical names. */
static void test_sim_dname_prefers_logical_name(void **state)
{
    struct scp_context_fixture *fixture = *state;

    assert_string_equal(sim_dname(&fixture->disk), "SYSDISK");
    fixture->disk.lname = NULL;
    assert_string_equal(sim_dname(&fixture->disk), "DQ");
    assert_string_equal(sim_dname(NULL), "");
}

/* Verify unit display names are formatted once and then cached. */
static void test_sim_uname_formats_and_caches_names(void **state)
{
    struct scp_context_fixture *fixture = *state;
    const char *first_name;
    const char *cached_name;

    first_name = sim_uname(&fixture->disk_units[1]);
    assert_string_equal(first_name, "SYSDISK1");

    cached_name = sim_uname(&fixture->disk_units[1]);
    assert_ptr_equal(cached_name, first_name);

    assert_string_equal(sim_uname(&fixture->reader_unit), "READER");
    assert_string_equal(sim_uname(NULL), "");
}

/* Verify replacing a cached unit name frees and stores the new value. */
static void test_sim_set_uname_replaces_cached_name(void **state)
{
    struct scp_context_fixture *fixture = *state;
    const char *first_name;
    const char *second_name;

    first_name = sim_set_uname(&fixture->disk_units[0], "CUSTOM0");
    assert_string_equal(first_name, "CUSTOM0");

    second_name = sim_set_uname(&fixture->disk_units[0], "RENAMED0");
    assert_string_equal(second_name, "RENAMED0");
    assert_string_equal(fixture->disk_units[0].uname, "RENAMED0");
}

/* Verify device lookup sees primary, logical, and internal device names. */
static void
test_find_dev_matches_primary_logical_and_internal_names(void **state)
{
    struct scp_context_fixture *fixture = *state;

    assert_ptr_equal(find_dev("DQ"), &fixture->disk);
    assert_ptr_equal(find_dev("SYSDISK"), &fixture->disk);
    assert_null(find_dev("CLOCK"));

    assert_int_equal(sim_register_internal_device(&fixture->internal), SCPE_OK);
    assert_ptr_equal(find_dev("CLK"), &fixture->internal);
    assert_ptr_equal(find_dev("CLOCK"), &fixture->internal);
}

/* Verify unit lookup matches bare, qualified, and cached unit names. */
static void
test_find_unit_matches_device_names_and_cached_unit_names(void **state)
{
    struct scp_context_fixture *fixture = *state;
    UNIT *uptr;
    DEVICE *dptr;

    uptr = NULL;
    dptr = find_unit("DQ", &uptr);
    assert_ptr_equal(dptr, &fixture->disk);
    assert_ptr_equal(uptr, &fixture->disk_units[0]);

    uptr = NULL;
    dptr = find_unit("SYSDISK1", &uptr);
    assert_ptr_equal(dptr, &fixture->disk);
    assert_ptr_equal(uptr, &fixture->disk_units[1]);

    uptr = NULL;
    dptr = find_unit("READER", &uptr);
    assert_ptr_equal(dptr, &fixture->reader);
    assert_ptr_equal(uptr, &fixture->reader_unit);

    assert_string_equal(sim_set_uname(&fixture->disk_units[1], "CUSTOM1"),
                        "CUSTOM1");
    uptr = NULL;
    dptr = find_unit("CUSTOM1", &uptr);
    assert_ptr_equal(dptr, &fixture->disk);
    assert_ptr_equal(uptr, &fixture->disk_units[1]);
}

/* Verify disabled devices are rejected by lookup and flagged as disabled. */
static void test_find_unit_rejects_disabled_devices(void **state)
{
    struct scp_context_fixture *fixture = *state;
    UNIT *uptr;

    uptr = (UNIT *)1;
    assert_null(find_unit("TTY", &uptr));
    assert_null(uptr);

    assert_true(qdisable(&fixture->disabled));
    assert_false(qdisable(&fixture->disk));
}

/* Verify internal-device registration ignores duplicates and main devices. */
static void test_register_internal_device_ignores_duplicates(void **state)
{
    struct scp_context_fixture *fixture = *state;

    assert_int_equal(sim_internal_device_count, 0);

    assert_int_equal(sim_register_internal_device(&fixture->internal), SCPE_OK);
    assert_int_equal(sim_internal_device_count, 1);
    assert_ptr_equal(sim_internal_devices[0], &fixture->internal);
    assert_null(sim_internal_devices[1]);

    assert_int_equal(sim_register_internal_device(&fixture->internal), SCPE_OK);
    assert_int_equal(sim_internal_device_count, 1);

    assert_int_equal(sim_register_internal_device(&fixture->disk), SCPE_OK);
    assert_int_equal(sim_internal_device_count, 1);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_sim_dname_prefers_logical_name,
                                        setup_scp_context_fixture,
                                        teardown_scp_context_fixture),
        cmocka_unit_test_setup_teardown(test_sim_uname_formats_and_caches_names,
                                        setup_scp_context_fixture,
                                        teardown_scp_context_fixture),
        cmocka_unit_test_setup_teardown(test_sim_set_uname_replaces_cached_name,
                                        setup_scp_context_fixture,
                                        teardown_scp_context_fixture),
        cmocka_unit_test_setup_teardown(
            test_find_dev_matches_primary_logical_and_internal_names,
            setup_scp_context_fixture, teardown_scp_context_fixture),
        cmocka_unit_test_setup_teardown(
            test_find_unit_matches_device_names_and_cached_unit_names,
            setup_scp_context_fixture, teardown_scp_context_fixture),
        cmocka_unit_test_setup_teardown(test_find_unit_rejects_disabled_devices,
                                        setup_scp_context_fixture,
                                        teardown_scp_context_fixture),
        cmocka_unit_test_setup_teardown(
            test_register_internal_device_ignores_duplicates,
            setup_scp_context_fixture, teardown_scp_context_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
