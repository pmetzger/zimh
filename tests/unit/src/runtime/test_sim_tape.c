#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "sim_defs.h"
#include "sim_tape.h"
#include "test_support.h"

struct sim_tape_fixture {
    DEVICE device;
    UNIT unit;
};

static int setup_sim_tape_fixture(void **state)
{
    struct sim_tape_fixture *fixture;

    fixture = calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    simh_test_init_device_unit(&fixture->device, &fixture->unit, "TAPE",
                               "TAPE0", DEV_DISABLE, UNIT_ATTABLE, 8, 1);
    fixture->unit.dynflags = MTUF_F_STD << UNIT_V_TAPE_FMT;

    *state = fixture;
    return 0;
}

static int teardown_sim_tape_fixture(void **state)
{
    struct sim_tape_fixture *fixture = *state;

    free(fixture);
    *state = NULL;
    return 0;
}

static void assert_show_output(t_stat (*show_fn)(FILE *, UNIT *, int32,
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

/* Verify the BOT/EOT/write-protect predicates track format, position,
   and tape flags. */
static void test_sim_tape_status_predicates_follow_unit_state(void **state)
{
    struct sim_tape_fixture *fixture = *state;

    fixture->unit.pos = 0;
    assert_true(sim_tape_bot(&fixture->unit));

    fixture->unit.pos = sizeof(t_mtrlnt);
    assert_false(sim_tape_bot(&fixture->unit));

    fixture->unit.pos = 0;
    MT_SET_INMRK(&fixture->unit);
    assert_false(sim_tape_bot(&fixture->unit));
    MT_CLR_INMRK(&fixture->unit);

    fixture->unit.capac = 0;
    assert_false(sim_tape_eot(&fixture->unit));

    fixture->unit.capac = 100;
    fixture->unit.pos = 99;
    assert_false(sim_tape_eot(&fixture->unit));
    fixture->unit.pos = 100;
    assert_true(sim_tape_eot(&fixture->unit));

    fixture->unit.flags = UNIT_ATTABLE;
    fixture->unit.dynflags = MTUF_F_STD << UNIT_V_TAPE_FMT;
    assert_false(sim_tape_wrp(&fixture->unit));

    fixture->unit.flags |= MTUF_WRP;
    assert_true(sim_tape_wrp(&fixture->unit));

    fixture->unit.flags = UNIT_ATTABLE;
    fixture->unit.dynflags = MTUF_F_TPC << UNIT_V_TAPE_FMT;
    assert_true(sim_tape_wrp(&fixture->unit));
}

/* Verify tape format changes update flags correctly and the show helper
   reports the selected format. */
static void test_sim_tape_set_and_show_format(void **state)
{
    struct sim_tape_fixture *fixture = *state;

    assert_int_equal(sim_tape_set_fmt(&fixture->unit, 0, "TPC", NULL), SCPE_OK);
    assert_int_equal(MT_GET_FMT(&fixture->unit), MTUF_F_TPC);
    assert_true((fixture->unit.flags & UNIT_RO) != 0);
    assert_show_output(sim_tape_show_fmt, &fixture->unit, "TPC format");

    assert_true(sim_tape_set_fmt(&fixture->unit, 0, "UNKNOWN", NULL) !=
                SCPE_OK);
}

/* Verify capacity parsing stores megabytes and the show helper formats
   the stored size. */
static void test_sim_tape_set_and_show_capacity(void **state)
{
    struct sim_tape_fixture *fixture = *state;

    assert_int_equal(sim_tape_set_capac(&fixture->unit, 0, "25", NULL),
                     SCPE_OK);
    assert_int_equal(fixture->unit.capac, ((t_addr)25 * 1000000));
    assert_show_output(sim_tape_show_capac, &fixture->unit, "capacity=25MB");

    fixture->unit.flags |= UNIT_ATT;
    assert_int_equal(sim_tape_set_capac(&fixture->unit, 0, "5", NULL),
                     SCPE_ALATT);
}

/* Verify density parsing works for both direct and validated-string
   entry paths, and that supported-density formatting is stable. */
static void test_sim_tape_density_helpers_validate_and_render(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    int32 valid_mask = MT_800_VALID | MT_1600_VALID | MT_6250_VALID;
    char density_list[64];

    assert_int_equal(
        sim_tape_set_dens(&fixture->unit, MT_DENS_1600, NULL, NULL), SCPE_OK);
    assert_int_equal(MT_DENS(fixture->unit.dynflags), MT_DENS_1600);
    assert_show_output(sim_tape_show_dens, &fixture->unit, "density=1600 bpi");

    assert_int_equal(sim_tape_set_dens(&fixture->unit, 0, "6250", &valid_mask),
                     SCPE_OK);
    assert_int_equal(MT_DENS(fixture->unit.dynflags), MT_DENS_6250);
    assert_show_output(sim_tape_show_dens, &fixture->unit, "density=6250 bpi");

    assert_int_equal(sim_tape_set_dens(&fixture->unit, 0, NULL, &valid_mask),
                     SCPE_MISVAL);
    assert_int_equal(sim_tape_set_dens(&fixture->unit, 0, "556", &valid_mask),
                     SCPE_ARG);

    assert_int_equal(sim_tape_density_supported(
                         density_list, sizeof(density_list), MT_1600_VALID),
                     SCPE_OK);
    assert_string_equal(density_list, "1600");

    assert_int_equal(sim_tape_density_supported(
                         density_list, sizeof(density_list), valid_mask),
                     SCPE_OK);
    assert_string_equal(density_list, "{800|1600|6250}");
}

/* Verify textual error translation covers named tape errors and generic
   numeric fallback. */
static void
test_sim_tape_error_text_covers_named_and_generic_errors(void **state)
{
    (void)state;

    assert_string_equal(sim_tape_error_text(MTSE_OK), "no error");
    assert_string_equal(sim_tape_error_text(MTSE_WRP), "write protected");
    assert_string_equal(sim_tape_error_text(MTSE_RUNAWAY), "tape runaway");
    assert_string_equal(sim_tape_error_text(MTSE_MAX_ERR + 5), "Error 16");
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_sim_tape_status_predicates_follow_unit_state,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(test_sim_tape_set_and_show_format,
                                        setup_sim_tape_fixture,
                                        teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(test_sim_tape_set_and_show_capacity,
                                        setup_sim_tape_fixture,
                                        teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_density_helpers_validate_and_render,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test(
            test_sim_tape_error_text_covers_named_and_generic_errors),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
