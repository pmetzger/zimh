#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "sim_defs.h"
#include "sim_disk.h"
#include "test_support.h"

struct sim_disk_fixture {
    DEVICE byte_device;
    UNIT byte_unit;
    DEVICE sector_device;
    UNIT sector_unit;
};

static int setup_sim_disk_fixture(void **state)
{
    struct sim_disk_fixture *fixture;

    fixture = calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    simh_test_init_device_unit(&fixture->byte_device, &fixture->byte_unit,
                               "DSKB", "DSKB0", DEV_DISABLE, UNIT_ATTABLE, 8,
                               1);
    fixture->byte_unit.flags |= DK_F_STD;

    simh_test_init_device_unit(&fixture->sector_device, &fixture->sector_unit,
                               "DSKS", "DSKS0", DEV_DISABLE | DEV_SECTORS,
                               UNIT_ATTABLE, 16, 1);
    fixture->sector_unit.flags |= DK_F_STD;

    *state = fixture;
    return 0;
}

static int teardown_sim_disk_fixture(void **state)
{
    struct sim_disk_fixture *fixture = *state;

    free(fixture);
    *state = NULL;
    return 0;
}

static void assert_disk_show_output(t_stat (*show_fn)(FILE *, UNIT *, int32,
                                                      const void *),
                                    UNIT *uptr, const char *expected)
{
    FILE *stream;
    char *text;
    size_t size;

    stream = tmpfile();
    assert_non_null(stream);

    assert_int_equal(show_fn(stream, uptr, 0, NULL), SCPE_OK);
    assert_int_equal(simh_test_read_stream(stream, &text, &size), 0);
    assert_string_equal(text, expected);

    free(text);
    fclose(stream);
}

/* Verify disk format selection updates the unit flags and that the show
   helper reflects the chosen format. */
static void test_sim_disk_set_and_show_format(void **state)
{
    struct sim_disk_fixture *fixture = *state;

    assert_int_equal(sim_disk_set_fmt(&fixture->byte_unit, 0, "SIMH", NULL),
                     SCPE_OK);
    assert_int_equal(DK_GET_FMT(&fixture->byte_unit), DKUF_F_STD);
    assert_disk_show_output(sim_disk_show_fmt, &fixture->byte_unit,
                            "SIMH format");

    assert_true(sim_disk_set_fmt(&fixture->byte_unit, 0, "UNKNOWN", NULL) !=
                SCPE_OK);
}

/* Verify byte-addressed disk capacity parsing stores byte counts and the
   show helper renders them with byte units. */
static void test_sim_disk_set_and_show_byte_capacity(void **state)
{
    struct sim_disk_fixture *fixture = *state;

    assert_int_equal(sim_disk_set_capac(&fixture->byte_unit, 0, "25", NULL),
                     SCPE_OK);
    assert_int_equal(fixture->byte_unit.capac, (t_addr)25000000);
    assert_disk_show_output(sim_disk_show_capac, &fixture->byte_unit,
                            "capacity=25MB");

    fixture->byte_unit.flags |= UNIT_ATT;
    assert_int_equal(sim_disk_set_capac(&fixture->byte_unit, 0, "5", NULL),
                     SCPE_ALATT);
}

/* Verify sector-addressed capacity parsing converts megabytes into
   sector counts for DEV_SECTORS devices. */
static void test_sim_disk_set_capacity_scales_for_sector_devices(void **state)
{
    struct sim_disk_fixture *fixture = *state;

    assert_int_equal(sim_disk_set_capac(&fixture->sector_unit, 0, "8", NULL),
                     SCPE_OK);
    assert_int_equal(fixture->sector_unit.capac, (t_addr)(8000000 / 512));
}

/* Verify availability and write-protect predicates track simple unit
   flag state without needing a live disk context. */
static void test_sim_disk_status_predicates_use_unit_flags(void **state)
{
    struct sim_disk_fixture *fixture = *state;

    assert_false(sim_disk_isavailable(&fixture->byte_unit));
    assert_false(sim_disk_wrp(&fixture->byte_unit));

    fixture->byte_unit.flags |= DKUF_WRP;
    assert_true(sim_disk_wrp(&fixture->byte_unit));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_sim_disk_set_and_show_format,
                                        setup_sim_disk_fixture,
                                        teardown_sim_disk_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_disk_set_and_show_byte_capacity, setup_sim_disk_fixture,
            teardown_sim_disk_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_disk_set_capacity_scales_for_sector_devices,
            setup_sim_disk_fixture, teardown_sim_disk_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_disk_status_predicates_use_unit_flags,
            setup_sim_disk_fixture, teardown_sim_disk_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
