#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include "sim_defs.h"
#include "sim_timer.h"

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

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_sim_timespec_diff_borrows_and_normalizes),
        cmocka_unit_test(test_sim_timespec_diff_handles_simple_difference),
        cmocka_unit_test(test_sim_timer_idle_capable_reports_default_state),
        cmocka_unit_test(test_sim_timer_default_query_helpers_are_stable),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
