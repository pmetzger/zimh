#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "sim_defs.h"
#include "sim_tmxr.h"

struct sim_tmxr_fixture {
    DEVICE device;
    UNIT unit;
    TMXR mux;
    TMLN line;
};

static int setup_sim_tmxr_fixture(void **state)
{
    struct sim_tmxr_fixture *fixture;

    fixture = calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    memset(&fixture->device, 0, sizeof(fixture->device));
    memset(&fixture->unit, 0, sizeof(fixture->unit));
    memset(&fixture->mux, 0, sizeof(fixture->mux));
    memset(&fixture->line, 0, sizeof(fixture->line));

    fixture->device.name = "TMXR";
    fixture->mux.dptr = &fixture->device;
    fixture->mux.ldsc = &fixture->line;
    fixture->mux.lines = 1;
    fixture->mux.uptr = &fixture->unit;

    fixture->line.mp = &fixture->mux;
    fixture->line.dptr = &fixture->device;
    fixture->line.rxbsz = 16;
    fixture->line.txbsz = 16;

    *state = fixture;
    return 0;
}

static int teardown_sim_tmxr_fixture(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;

    free(fixture->line.lpb);
    free(fixture);
    *state = NULL;
    return 0;
}

/* Verify loopback toggling updates the line flag and allocates or frees
   the private loopback buffer. */
static void test_tmxr_loopback_toggle_updates_buffer_state(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;

    assert_false(tmxr_get_line_loopback(&fixture->line));
    assert_null(fixture->line.lpb);

    assert_int_equal(tmxr_set_line_loopback(&fixture->line, TRUE), SCPE_OK);
    assert_true(tmxr_get_line_loopback(&fixture->line));
    assert_non_null(fixture->line.lpb);
    assert_int_equal(fixture->line.lpbsz, fixture->line.rxbsz);
    assert_true(fixture->line.ser_connect_pending);

    assert_int_equal(tmxr_set_line_loopback(&fixture->line, FALSE), SCPE_OK);
    assert_false(tmxr_get_line_loopback(&fixture->line));
    assert_null(fixture->line.lpb);
    assert_int_equal(fixture->line.lpbsz, 0);
}

/* Verify half-duplex mode toggles independently of other line state. */
static void test_tmxr_halfduplex_toggle_round_trips(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;

    assert_false(tmxr_get_line_halfduplex(&fixture->line));
    assert_int_equal(tmxr_set_line_halfduplex(&fixture->line, TRUE), SCPE_OK);
    assert_true(tmxr_get_line_halfduplex(&fixture->line));
    assert_int_equal(tmxr_set_line_halfduplex(&fixture->line, FALSE), SCPE_OK);
    assert_false(tmxr_get_line_halfduplex(&fixture->line));
}

/* Verify receive and transmit queue helpers account for both linear and
   wrapped circular-buffer state. */
static void test_tmxr_queue_length_helpers_handle_wraparound(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;

    fixture->line.rxbpr = 2;
    fixture->line.rxbpi = 6;
    assert_int_equal(tmxr_rqln(&fixture->line), 4);
    assert_int_equal(tmxr_input_pending_ln(&fixture->line), 4);

    fixture->line.rxbpr = 10;
    fixture->line.rxbpi = 3;
    assert_int_equal(tmxr_rqln(&fixture->line), 9);
    assert_int_equal(tmxr_input_pending_ln(&fixture->line), -7);

    fixture->line.txbpr = 5;
    fixture->line.txbpi = 9;
    assert_int_equal(tmxr_tqln(&fixture->line), 4);

    fixture->line.txbpr = 12;
    fixture->line.txbpi = 3;
    assert_int_equal(tmxr_tqln(&fixture->line), 7);

    fixture->line.txppsize = 20;
    fixture->line.txppoffset = 5;
    assert_int_equal(tmxr_tpqln(&fixture->line), 15);
    assert_true(tmxr_tpbusyln(&fixture->line));

    fixture->line.txppoffset = fixture->line.txppsize;
    assert_int_equal(tmxr_tpqln(&fixture->line), 0);
    assert_false(tmxr_tpbusyln(&fixture->line));
}

/* Verify poll-interval validation rejects zero and stores accepted
   values on the multiplexer descriptor. */
static void test_tmxr_connection_poll_interval_validates_zero(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;

    assert_int_equal(tmxr_connection_poll_interval(&fixture->mux, 0), SCPE_ARG);
    assert_int_equal(tmxr_connection_poll_interval(&fixture->mux, 5), SCPE_OK);
    assert_int_equal(fixture->mux.poll_interval, 5);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_tmxr_loopback_toggle_updates_buffer_state,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(test_tmxr_halfduplex_toggle_round_trips,
                                        setup_sim_tmxr_fixture,
                                        teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_queue_length_helpers_handle_wraparound,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_connection_poll_interval_validates_zero,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
