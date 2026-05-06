#include <stddef.h>
#include <stdint.h>

#include "test_cmocka.h"

#include "sim_defs.h"
#include "sim_timer.h"

static int simh_test_clock_calls = 0;
static int simh_test_sleep_calls = 0;
static int simh_test_last_clock_id = -1;
static int simh_test_clock_status = 0;
static struct timespec simh_test_last_sleep_req = {0};
static struct timespec simh_test_clock_values[4] = {{0}};
static size_t simh_test_clock_value_count = 0;
static size_t simh_test_clock_value_index = 0;

/* Return scripted timespec values through the shared time wrapper hook. */
static int simh_test_clock_gettime_stub (int clock_id, struct timespec *tp)
{
    ++simh_test_clock_calls;
    simh_test_last_clock_id = clock_id;
    if ((tp != NULL) && (simh_test_clock_value_index < simh_test_clock_value_count))
        *tp = simh_test_clock_values[simh_test_clock_value_index++];
    if (simh_test_clock_status != 0)
        errno = EINVAL;
    return simh_test_clock_status;
}

/* Record the requested sleep interval and return success. */
static int simh_test_nanosleep_stub (const struct timespec *req,
                                     struct timespec *rem)
{
    ++simh_test_sleep_calls;
    if (req != NULL)
        simh_test_last_sleep_req = *req;
    if (rem != NULL) {
        rem->tv_sec = 0;
        rem->tv_nsec = 0;
    }
    return 0;
}

/* Reset timer test hooks and scripted values between cases. */
static int setup_sim_timer_fixture (void **state)
{
    (void)state;

    simh_test_clock_calls = 0;
    simh_test_sleep_calls = 0;
    simh_test_last_clock_id = -1;
    simh_test_clock_status = 0;
    simh_test_last_sleep_req = (struct timespec){0};
    simh_test_clock_value_count = 0;
    simh_test_clock_value_index = 0;
    sim_time_reset_test_hooks ();
    errno = 0;
    return 0;
}

/* Restore the default shared time hooks after a test case completes. */
static int teardown_sim_timer_fixture (void **state)
{
    (void)state;

    sim_time_reset_test_hooks ();
    return 0;
}

/* Verify timespec subtraction borrows from seconds when nanoseconds
   would otherwise go negative. */
static void test_sim_timespec_diff_borrows_and_normalizes(void **state)
{
    struct timespec minuend = {.tv_sec = 5, .tv_nsec = 100};
    struct timespec subtrahend = {.tv_sec = 2, .tv_nsec = 200};
    struct timespec diff;

    (void)state;

    sim_timespec_diff(&diff, &minuend, &subtrahend);
    assert_int_equal(diff.tv_sec, 2);
    assert_int_equal(diff.tv_nsec, 999999900L);
}

/* Verify timespec subtraction leaves already-normalized values alone. */
static void test_sim_timespec_diff_handles_simple_difference(void **state)
{
    struct timespec minuend = {.tv_sec = 12, .tv_nsec = 800000000};
    struct timespec subtrahend = {.tv_sec = 7, .tv_nsec = 300000000};
    struct timespec diff;

    (void)state;

    sim_timespec_diff(&diff, &minuend, &subtrahend);
    assert_int_equal(diff.tv_sec, 5);
    assert_int_equal(diff.tv_nsec, 500000000L);
}

/* Verify deadline construction uses CLOCK_REALTIME and normalizes carry. */
static void test_sim_timer_deadline_msec_builds_absolute_deadline(void **state)
{
    struct timespec deadline = {0};

    (void)state;

    simh_test_clock_values[0] = (struct timespec){.tv_sec = 10,
                                                  .tv_nsec = 900000000L};
    simh_test_clock_value_count = 1;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub, NULL);

    assert_int_equal (sim_timer_deadline_msec (&deadline, 250), 0);
    assert_int_equal (simh_test_clock_calls, 1);
    assert_int_equal (simh_test_last_clock_id, CLOCK_REALTIME);
    assert_int_equal (deadline.tv_sec, 11);
    assert_int_equal (deadline.tv_nsec, 150000000L);
}

/* Verify deadline construction reports wrapper failure unchanged. */
static void test_sim_timer_deadline_msec_reports_clock_failure(void **state)
{
    struct timespec deadline = {.tv_sec = 1, .tv_nsec = 2};

    (void)state;

    simh_test_clock_status = -1;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub, NULL);

    assert_int_equal (sim_timer_deadline_msec (&deadline, 5), -1);
    assert_int_equal (simh_test_clock_calls, 1);
    assert_int_equal (deadline.tv_sec, 1);
    assert_int_equal (deadline.tv_nsec, 2);
}

/* Verify the timer library reports its uninitialized default capability
   state without requiring timer startup. */
static void test_sim_timer_idle_capable_reports_default_state(void **state)
{
    uint32 host_sleep_ms = 1234;
    uint32 host_tick_ms = 5678;

    (void)state;

    assert_false(sim_timer_idle_capable(&host_sleep_ms, &host_tick_ms));
    assert_int_equal(host_sleep_ms, 0);
    assert_int_equal(host_tick_ms, 0);
}

/* Verify public calibrated-timer queries return the documented defaults
   before any runtime timer initialization occurs. */
static void test_sim_timer_default_query_helpers_are_stable(void **state)
{
    (void)state;

    assert_int_equal(sim_rtcn_calibrated_tmr(), 0);
    assert_int_equal(sim_rtcn_tick_size(0), 10000);
    assert_int_equal(sim_rtcn_tick_size(SIM_NTIMERS), 10000);
    assert_true(sim_timer_inst_per_sec() >= 0.0);
}

/* Verify sim_os_msec uses the shared clock wrapper and ms conversion. */
static void test_sim_os_msec_uses_shared_clock_wrapper (void **state)
{
    (void)state;

    simh_test_clock_values[0] = (struct timespec){.tv_sec = 12,
                                                  .tv_nsec = 345000000L};
    simh_test_clock_value_count = 1;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub, NULL);

    assert_int_equal (sim_os_msec (), 12345);
    assert_int_equal (simh_test_clock_calls, 1);
    assert_int_equal (simh_test_last_clock_id, CLOCK_REALTIME);
}

/* Verify epoch-scale seconds produce the expected wrapping millisecond
   tick value. */
static void test_sim_os_msec_handles_epoch_scale_seconds (void **state)
{
    const time_t epoch_seconds = 1777949713;
    const long epoch_nsecs = 456000000L;
    const uint32 expected =
        (uint32)(((uint64_t)epoch_seconds * 1000u) +
                 ((uint64_t)epoch_nsecs / 1000000u));

    (void)state;

    simh_test_clock_values[0] = (struct timespec){.tv_sec = epoch_seconds,
                                                  .tv_nsec = epoch_nsecs};
    simh_test_clock_value_count = 1;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub, NULL);

    assert_int_equal (sim_os_msec (), expected);
    assert_int_equal (simh_test_clock_calls, 1);
    assert_int_equal (simh_test_last_clock_id, CLOCK_REALTIME);
}

/* Verify sim_os_ms_sleep uses the shared clock and nanosleep wrappers. */
static void test_sim_os_ms_sleep_uses_shared_time_wrappers (void **state)
{
    (void)state;

    simh_test_clock_values[0] = (struct timespec){.tv_sec = 1,
                                                  .tv_nsec = 100000000L};
    simh_test_clock_values[1] = (struct timespec){.tv_sec = 1,
                                                  .tv_nsec = 190000000L};
    simh_test_clock_value_count = 2;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub,
                             simh_test_nanosleep_stub);

    assert_int_equal (sim_os_ms_sleep (50), 90);
    assert_int_equal (simh_test_clock_calls, 2);
    assert_int_equal (simh_test_sleep_calls, 1);
    assert_int_equal (simh_test_last_sleep_req.tv_sec, 0);
    assert_int_equal (simh_test_last_sleep_req.tv_nsec, 50000000L);
}

/* Verify sim_timenow_double reads time through the shared clock wrapper. */
static void test_sim_timenow_double_uses_shared_clock_wrapper (void **state)
{
    double now;

    (void)state;

    simh_test_clock_values[0] = (struct timespec){.tv_sec = 42,
                                                  .tv_nsec = 125000000L};
    simh_test_clock_value_count = 1;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub, NULL);

    now = sim_timenow_double ();

    assert_int_equal (simh_test_clock_calls, 1);
    assert_int_equal (simh_test_last_clock_id, CLOCK_REALTIME);
    assert_float_equal (now, 42.125, 0.000001);
}

/* Verify ROM delay reads tolerate high-bit input words without changing the
   caller-visible value. */
static void test_sim_rom_read_with_delay_accepts_high_bit_words (void **state)
{
    const uint32 values[] = {
        0x80000000u,
        0x89abcdefu,
        0xffffffffu,
    };
    size_t i;

    (void)state;

    sim_set_rom_delay_factor (1);
    for (i = 0; i < sizeof (values) / sizeof (values[0]); ++i)
        assert_int_equal (sim_rom_read_with_delay (values[i]), values[i]);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_sim_timespec_diff_borrows_and_normalizes,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timespec_diff_handles_simple_difference,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timer_deadline_msec_builds_absolute_deadline,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timer_deadline_msec_reports_clock_failure,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timer_idle_capable_reports_default_state,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timer_default_query_helpers_are_stable,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_os_msec_uses_shared_clock_wrapper,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_os_msec_handles_epoch_scale_seconds,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_os_ms_sleep_uses_shared_time_wrappers,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timenow_double_uses_shared_clock_wrapper,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_rom_read_with_delay_accepts_high_bit_words,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
