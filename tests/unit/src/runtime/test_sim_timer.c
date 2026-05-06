#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "test_cmocka.h"

#include "scp.h"
#include "sim_defs.h"
#include "sim_timer_internal.h"
#include "sim_timer.h"
#include "test_scp_fixture.h"
#include "test_simh_personality.h"
#include "test_support.h"

static int simh_test_clock_calls = 0;
static int simh_test_sleep_calls = 0;
static int simh_test_last_clock_id = -1;
static int simh_test_clock_status = 0;
static struct timespec simh_test_last_sleep_req = {0};
static struct timespec simh_test_clock_values[4] = {{0}};
static size_t simh_test_clock_value_count = 0;
static size_t simh_test_clock_value_index = 0;
static uint64_t simh_test_clock_auto_msec = 0;
static uint32 simh_test_clock_auto_step_msec = 0;

struct sim_timer_activation_fixture {
    DEVICE clock_device;
    DEVICE target_device;
    DEVICE *devices[3];
    UNIT clock_unit;
    UNIT target_unit;
    t_bool saved_sim_is_running;
};

/* Return scripted timespec values through the shared time wrapper hook. */
static int simh_test_clock_gettime_stub (int clock_id, struct timespec *tp)
{
    ++simh_test_clock_calls;
    simh_test_last_clock_id = clock_id;
    if ((tp != NULL) &&
        (simh_test_clock_value_index < simh_test_clock_value_count))
        *tp = simh_test_clock_values[simh_test_clock_value_index++];
    else if ((tp != NULL) && (simh_test_clock_auto_step_msec != 0)) {
        simh_test_clock_auto_msec += simh_test_clock_auto_step_msec;
        tp->tv_sec = (time_t)(simh_test_clock_auto_msec / 1000u);
        tp->tv_nsec = (long)((simh_test_clock_auto_msec % 1000u) * 1000000u);
    }
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
    simh_test_clock_auto_msec = 0;
    simh_test_clock_auto_step_msec = 0;
    sim_time_reset_test_hooks ();
    sim_timer_reset_test_state ();
    sim_is_running = FALSE;
    sim_set_rom_delay_factor (1);
    errno = 0;
    return 0;
}

/* Restore the default shared time hooks after a test case completes. */
static int teardown_sim_timer_fixture (void **state)
{
    (void)state;

    while (sim_clock_queue != QUEUE_LIST_END)
        sim_cancel (sim_clock_queue);
    sim_time_reset_test_hooks ();
    return 0;
}

static t_stat sim_timer_test_unit_svc (UNIT *uptr)
{
    (void)uptr;

    return SCPE_OK;
}

/* Read and return the complete text written to a temporary stream. */
static char *sim_timer_read_stream_text (FILE *stream)
{
    char *text = NULL;
    size_t size = 0;

    assert_int_equal (fflush (stream), 0);
    rewind (stream);
    assert_int_equal (simh_test_read_stream (stream, &text, &size), 0);
    assert_non_null (text);
    return text;
}

/* Build a calibrated-timer fixture for wall-clock activation tests. */
static int setup_sim_timer_activation_fixture (void **state)
{
    struct sim_timer_activation_fixture *fixture;

    assert_int_equal (setup_sim_timer_fixture (state), 0);

    fixture = calloc (1, sizeof (*fixture));
    assert_non_null (fixture);

    simh_test_init_device_unit (&fixture->clock_device, &fixture->clock_unit,
                                "CLK", "CLK0", 0, 0, 8, 1);
    simh_test_init_device_unit (&fixture->target_device, &fixture->target_unit,
                                "TMR", "TMR0", 0, 0, 8, 1);
    fixture->clock_unit.action = sim_timer_test_unit_svc;
    fixture->target_unit.action = sim_timer_test_unit_svc;
    fixture->devices[0] = &fixture->clock_device;
    fixture->devices[1] = &fixture->target_device;
    fixture->devices[2] = NULL;

    assert_int_equal (
        simh_test_install_devices ("zimh-unit-sim-timer", fixture->devices), 0);
    assert_false (sim_timer_init ());
    assert_int_equal (
        sim_rtcn_init_unit_ticks (&fixture->clock_unit, 100, 0, 100), 100);
    fixture->saved_sim_is_running = sim_is_running;

    *state = fixture;
    return 0;
}

/* Tear down timer fixture state that may be queued by activation tests. */
static int teardown_sim_timer_activation_fixture (void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;

    sim_cancel (&fixture->target_unit);
    sim_register_clock_unit_tmr (NULL, 0);
    sim_is_running = fixture->saved_sim_is_running;
    simh_test_reset_simulator_state ();
    free (fixture);
    *state = NULL;
    assert_int_equal (teardown_sim_timer_fixture (state), 0);
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

/* Verify timer initialization can be driven deterministically through the
   shared time hooks and records the measured host timing capability. */
static void test_sim_timer_init_measures_host_timing_with_hooks (void **state)
{
    uint32 host_sleep_ms = 0;
    uint32 host_tick_ms = 0;

    (void)state;

    simh_test_clock_auto_step_msec = 1;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub,
                             simh_test_nanosleep_stub);

    assert_false (sim_timer_init ());
    assert_true (simh_test_clock_calls > 400);
    assert_true (simh_test_sleep_calls > 200);
    assert_true (sim_timer_idle_capable (&host_sleep_ms, &host_tick_ms));
    assert_int_equal (host_sleep_ms, 1);
    assert_int_equal (host_tick_ms, 1);
    assert_ptr_equal (rtcs[0].timer_unit, &sim_timer_units[0]);
    assert_ptr_equal (rtcs[SIM_NTIMERS].timer_unit,
                      &sim_timer_units[SIM_NTIMERS]);
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

/* Verify invalid timer initialization requests are bounded to the sanitized
   default delay without touching out-of-range timer state. */
static void test_sim_rtcn_init_rejects_invalid_timer_numbers (void **state)
{
    (void)state;

    assert_int_equal (sim_rtcn_init_unit_ticks (NULL, 0, -1, 0), 1);
    assert_int_equal (
        sim_rtcn_init_unit_ticks (NULL, 25, SIM_NTIMERS + 1, 0), 25);
    assert_int_equal (sim_rtcn_calb (100, -1), 10000);
    assert_int_equal (sim_rtcn_calb (100, SIM_NTIMERS + 1), 10000);
}

/* Verify timer-unit initialization records the active clock contract and
   resets the one-second calibration counters. */
static void test_sim_rtcn_init_unit_ticks_records_clock_state (void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;
    RTC *rtc = &rtcs[0];

    assert_ptr_equal (rtc->clock_unit, &fixture->clock_unit);
    assert_ptr_equal (rtc->timer_unit, &sim_timer_units[0]);
    assert_int_equal (fixture->clock_unit.dynflags & UNIT_TMR_UNIT,
                      UNIT_TMR_UNIT);
    assert_int_equal (rtc->ticks, 0);
    assert_int_equal (rtc->hz, 100);
    assert_int_equal (rtc->based, 100);
    assert_int_equal (rtc->currd, 100);
    assert_int_equal (rtc->initd, 100);
    assert_int_equal (rtc->elapsed, 0);
    assert_int_equal (rtc->calib_initializations, 1);
}

/* Verify legacy timer-0 wrappers still delegate to the numbered timer
   interfaces. */
static void test_sim_rtc_legacy_wrappers_use_timer_zero (void **state)
{
    RTC *rtc = &rtcs[0];

    (void)state;

    rtc->currd = 0;
    rtc->initd = 0;
    assert_int_equal (sim_rtc_init (200), 200);
    assert_int_equal (rtc->currd, 200);
    assert_int_equal (rtc->initd, 200);
    assert_int_equal (sim_rtc_calb (100), 200);
    assert_int_equal (sim_rtcn_calb_tick (0), 200);
}

/* Verify calibration ticks before the configured one-second boundary only
   count ticks and return the current calibrated delay. */
static void test_sim_rtcn_calb_counts_ticks_before_calibration (void **state)
{
    RTC *rtc = &rtcs[0];

    (void)state;

    assert_int_equal (sim_rtcn_calb (100, 0), 100);
    assert_int_equal (rtc->ticks, 1);
    assert_int_equal (rtc->elapsed, 0);
    assert_int_equal (rtc->calibrations, 0);
}

/* Verify the one-second calibration path updates elapsed time and keeps a
   stable current delay when wall time and virtual time agree. */
static void test_sim_rtcn_calb_updates_at_one_second_boundary (void **state)
{
    RTC *rtc = &rtcs[0];

    (void)state;

    rtc->ticks = 99;
    rtc->rtime = 1000;
    rtc->vtime = 1000;
    rtc->gtime = sim_gtime ();
    simh_test_clock_values[0] = (struct timespec){.tv_sec = 2,
                                                  .tv_nsec = 0};
    simh_test_clock_value_count = 1;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub, NULL);

    assert_int_equal (sim_rtcn_calb (100, 0), 100);
    assert_int_equal (rtc->ticks, 0);
    assert_int_equal (rtc->elapsed, 1);
    assert_int_equal (rtc->calibrations, 1);
    assert_int_equal (rtc->based, 100);
    assert_int_equal (rtc->currd, 100);
    assert_int_equal (rtc->nxintv, 1000);
}

/* Verify calibration treats a wrapping millisecond clock as running
   backwards and leaves the current delay unchanged. */
static void test_sim_rtcn_calb_handles_backwards_host_time (void **state)
{
    RTC *rtc = &rtcs[0];

    (void)state;

    rtc->ticks = 99;
    rtc->rtime = 1000;
    rtc->vtime = 1000;
    simh_test_clock_values[0] = (struct timespec){.tv_sec = 0,
                                                  .tv_nsec = 900000000L};
    simh_test_clock_value_count = 1;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub, NULL);

    assert_int_equal (sim_rtcn_calb (100, 0), 100);
    assert_int_equal (rtc->clock_calib_backwards, 1);
    assert_int_equal (rtc->rtime, 900);
    assert_int_equal (rtc->vtime, 900);
    assert_int_equal (rtc->based, 100);
    assert_int_equal (rtc->currd, 100);
}

/* Verify calibration ignores large host-clock gaps and restarts from the
   observed wall-clock time. */
static void test_sim_rtcn_calb_ignores_large_host_time_gap (void **state)
{
    RTC *rtc = &rtcs[0];

    (void)state;

    rtc->ticks = 99;
    rtc->rtime = 1000;
    rtc->vtime = 1000;
    simh_test_clock_values[0] = (struct timespec){.tv_sec = 32,
                                                  .tv_nsec = 0};
    simh_test_clock_value_count = 1;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub, NULL);

    assert_int_equal (sim_rtcn_calb (100, 0), 100);
    assert_int_equal (rtc->clock_calib_gap2big, 1);
    assert_int_equal (rtc->rtime, 32000);
    assert_int_equal (rtc->vtime, 32000);
    assert_int_equal (rtc->nxintv, 1000);
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

/* Verify the public time query wrappers use the shared clock hook and return
   seconds through both the return value and optional output pointer. */
static void test_sim_rtcn_get_time_and_sim_get_time_use_clock_wrapper (
    void **state)
{
    struct timespec now = {0};
    time_t seconds = 0;

    (void)state;

    simh_test_clock_values[0] = (struct timespec){.tv_sec = 123,
                                                  .tv_nsec = 456000000L};
    simh_test_clock_values[1] = (struct timespec){.tv_sec = 124,
                                                  .tv_nsec = 0};
    simh_test_clock_value_count = 2;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub, NULL);

    sim_rtcn_get_time (&now, 0);
    assert_int_equal (now.tv_sec, 123);
    assert_int_equal (now.tv_nsec, 456000000L);
    assert_int_equal (sim_get_time (&seconds), 124);
    assert_int_equal (seconds, 124);
    assert_int_equal (simh_test_clock_calls, 2);
    assert_int_equal (simh_test_last_clock_id, CLOCK_REALTIME);
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

/* Verify a deferred wall-clock activation tolerates a calibrated timer whose
   assist unit is not currently queued. */
static void test_sim_timer_activate_after_handles_inactive_calibrated_tick (
    void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;

    assert_false (sim_is_active (&fixture->clock_unit));

    assert_int_equal (
        sim_timer_activate_after (&fixture->target_unit, 1000.0), SCPE_OK);

    assert_true (sim_is_active (&fixture->target_unit));
    assert_float_equal (fixture->target_unit.usecs_remaining, 1000.0,
                        0.000001);
}

/* Verify long wall-clock activations are bounded before converting the
   instruction delay to the event queue's int32 representation. */
static void test_sim_timer_activate_after_bounds_long_running_delay (
    void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;

    sim_is_running = TRUE;

    assert_int_equal (
        sim_timer_activate_after (&fixture->target_unit, 30000000000000.0),
        SCPE_OK);

    assert_true (sim_is_active (&fixture->target_unit));
    assert_int_equal (fixture->target_unit.time, 0x7fffffff);
    assert_int_equal (sim_interval, 0x7fffffff);
}

/* Verify small non-zero wall-clock activations still get one event-queue
   instruction so they cannot silently become inactive. */
static void test_sim_timer_activate_after_preserves_minimum_delay (
    void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;

    assert_int_equal (sim_timer_activate_after (&fixture->target_unit, 1.0),
                      SCPE_OK);

    assert_true (sim_is_active (&fixture->target_unit));
    assert_int_equal (fixture->target_unit.time, 1);
    assert_int_equal (sim_interval, 1);
}

/* Verify a clock unit activation schedules the timer assist unit and can be
   queried and canceled through the clock-unit wrapper API. */
static void test_sim_timer_clock_unit_activation_uses_assist_unit (
    void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;

    assert_int_equal (sim_timer_activate_after (&fixture->clock_unit, 1000.0),
                      SCPE_OK);

    assert_true (sim_is_active (&fixture->clock_unit));
    assert_true (sim_is_active (&sim_timer_units[0]));
    assert_true (sim_timer_is_active (&fixture->clock_unit));
    assert_float_equal (sim_activate_time_usecs (&fixture->clock_unit),
                        1000.0, 0.000001);
    assert_int_equal (sim_timer_cancel (&fixture->clock_unit), SCPE_OK);
    assert_false (sim_timer_is_active (&fixture->clock_unit));
}

/* Verify direct activation time helpers report inactive units and active
   standard queue units consistently. */
static void test_sim_timer_activation_time_queries_standard_queue (
    void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;

    assert_int_equal (_sim_timer_activate_time (&fixture->target_unit), -1);
    assert_float_equal (sim_timer_activate_time_usecs (&fixture->target_unit),
                        -1.0, 0.000001);

    sim_is_running = TRUE;
    assert_int_equal (sim_timer_activate_after (&fixture->target_unit, 1000.0),
                      SCPE_OK);

    assert_int_equal (sim_activate_time (&fixture->target_unit), 11);
    assert_float_equal (sim_activate_time_usecs (&fixture->target_unit),
                        1000.0, 0.000001);
}

/* Verify coscheduled clock events are stored as delta ticks and preserve
   ordering when inserted at the head, middle, and tail. */
static void test_sim_clock_coschedule_orders_delta_tick_queue (void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;
    UNIT first = {0};
    UNIT second = {0};
    UNIT third = {0};

    first.dptr = &fixture->target_device;
    second.dptr = &fixture->target_device;
    third.dptr = &fixture->target_device;

    assert_int_equal (sim_clock_coschedule_tmr (&second, 0, 5), SCPE_OK);
    assert_int_equal (sim_clock_coschedule_tmr (&first, 0, 2), SCPE_OK);
    assert_int_equal (sim_clock_coschedule_tmr (&third, 0, 8), SCPE_OK);

    assert_ptr_equal (rtcs[0].clock_cosched_queue, &first);
    assert_ptr_equal (first.next, &second);
    assert_ptr_equal (second.next, &third);
    assert_ptr_equal (third.next, QUEUE_LIST_END);
    assert_int_equal (rtcs[0].cosched_interval, 2);
    assert_int_equal (first.time, 2);
    assert_int_equal (second.time, 3);
    assert_int_equal (third.time, 3);

    assert_int_equal (sim_cancel (&second), SCPE_OK);
    assert_ptr_equal (first.next, &third);
    assert_int_equal (third.time, 6);
    assert_int_equal (sim_cancel (&third), SCPE_OK);
    assert_int_equal (sim_cancel (&first), SCPE_OK);
    assert_ptr_equal (rtcs[0].clock_cosched_queue, QUEUE_LIST_END);
}

/* Verify coscheduling rejects negative ticks and falls back to the standard
   queue for invalid timer numbers. */
static void test_sim_clock_coschedule_handles_invalid_requests (void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;

    assert_int_equal (
        sim_clock_coschedule_tmr (&fixture->target_unit, 0, -1), SCPE_ARG);

    assert_int_equal (sim_clock_coschedule_tmr (&fixture->target_unit,
                                               SIM_NTIMERS + 1, 2),
                      SCPE_OK);
    assert_true (sim_is_active (&fixture->target_unit));
    assert_int_equal (sim_activate_time (&fixture->target_unit), 20001);
}

/* Verify the rounded public coschedule wrappers use the calibrated timer and
   the absolute forms replace an existing coscheduled request. */
static void test_sim_clock_coschedule_wrappers_round_to_ticks (void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;

    assert_int_equal (sim_clock_coschedule (&fixture->target_unit, 249),
                      SCPE_OK);
    assert_ptr_equal (rtcs[0].clock_cosched_queue, &fixture->target_unit);
    assert_int_equal (fixture->target_unit.time, 2);

    assert_int_equal (sim_clock_coschedule_abs (&fixture->target_unit, 351),
                      SCPE_OK);
    assert_ptr_equal (rtcs[0].clock_cosched_queue, &fixture->target_unit);
    assert_int_equal (fixture->target_unit.time, 4);
    assert_int_equal (
        sim_clock_coschedule_tmr_abs (&fixture->target_unit, 0, 1), SCPE_OK);
    assert_int_equal (fixture->target_unit.time, 1);
    assert_int_equal (sim_cancel (&fixture->target_unit), SCPE_OK);
}

/* Verify the clock queue show helper reports coscheduled entries. */
static void test_sim_show_clock_queues_reports_coscheduled_entries (
    void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;
    char *text;
    FILE *stream;

    assert_int_equal (sim_clock_coschedule_tmr (&fixture->target_unit, 0, 3),
                      SCPE_OK);

    stream = tmpfile ();
    assert_non_null (stream);
    assert_int_equal (
        sim_show_clock_queues (stream, NULL, NULL, 0, NULL), SCPE_OK);
    text = sim_timer_read_stream_text (stream);

    assert_non_null (strstr (text, "co-schedule event queue status"));
    assert_non_null (strstr (text, "on next tick"));

    free (text);
    fclose (stream);
    assert_int_equal (sim_cancel (&fixture->target_unit), SCPE_OK);
}

/* Verify idling exits safely when no calibrated timer exists and accounts for
   the instruction cycle already consumed by the caller. */
static void test_sim_idle_without_calibrated_timer_counts_single_cycle (
    void **state)
{
    (void)state;

    sim_interval = 100;
    sim_idle_enab = FALSE;
    rtcs[0].elapsed = SIM_IDLE_STDFLT;

    assert_false (sim_idle (0, 7));
    assert_int_equal (sim_interval, 93);
}

/* Verify idling rejects out-of-range timer numbers before indexing the timer
   state array. */
static void test_sim_idle_rejects_invalid_timer_number (void **state)
{
    (void)state;

    sim_interval = 100;
    sim_idle_enab = TRUE;

    assert_false (sim_idle (SIM_NTIMERS + 1, 7));
    assert_int_equal (sim_interval, 93);
}

/* Verify idling performs a deterministic sleep when the timer is stable and
   the pending queue entry is marked idle-capable. */
static void test_sim_idle_sleeps_for_idle_capable_event (void **state)
{
    struct sim_timer_activation_fixture *fixture = *state;

    simh_test_clock_auto_step_msec = 1;
    sim_time_set_test_hooks (simh_test_clock_gettime_stub,
                             simh_test_nanosleep_stub);
    assert_int_equal (sim_timer_init (), 0);
    assert_int_equal (
        sim_rtcn_init_unit_ticks (&fixture->clock_unit, 100, 0, 100), 100);
    simh_test_clock_calls = 0;
    simh_test_sleep_calls = 0;
    simh_test_clock_auto_step_msec = 0;
    simh_test_clock_values[0] = (struct timespec){.tv_sec = 10,
                                                  .tv_nsec = 0};
    simh_test_clock_values[1] = (struct timespec){.tv_sec = 10,
                                                  .tv_nsec = 100000000L};
    simh_test_clock_value_count = 2;
    simh_test_clock_value_index = 0;
    rtcs[0].elapsed = SIM_IDLE_STDFLT;
    sim_idle_enab = TRUE;
    fixture->target_unit.flags = UNIT_IDLE;
    assert_int_equal (sim_activate (&fixture->target_unit, 1000), SCPE_OK);

    assert_true (sim_idle (0, 0));
    assert_int_equal (simh_test_sleep_calls, 1);
    assert_int_equal (simh_test_last_sleep_req.tv_sec, 0);
    assert_int_equal (simh_test_last_sleep_req.tv_nsec, 100000000L);
    assert_int_equal (rtcs[0].clock_time_idled, 100);
    assert_true (sim_interval < 1000);
}

/* Verify the user-facing idle setters accept the documented range and reject
   values below the minimum stability threshold. */
static void test_sim_set_idle_validates_stability_range (void **state)
{
    (void)state;

    assert_int_equal (SCPE_BARE_STATUS (sim_set_idle (NULL, 0, "1", NULL)),
                      SCPE_ARG);
    assert_false (sim_idle_enab);
    assert_int_equal (sim_set_idle (NULL, 0, "2", NULL), SCPE_OK);
    assert_true (sim_idle_enab);
    assert_int_equal (sim_clr_idle (NULL, 0, NULL, NULL), SCPE_OK);
    assert_false (sim_idle_enab);
}

/* Verify idle and catchup show/set helpers emit their current user-facing
   status text and accept comma-separated SET CLOCK modifiers. */
static void test_sim_timer_set_and_show_helpers (void **state)
{
    char *text;
    FILE *stream;

    (void)state;

    assert_int_equal (sim_set_timers (0, NULL), SCPE_2FARG);
    assert_int_equal (sim_set_timers (0, "BOGUS"), SCPE_NOPARAM);
    assert_int_equal (SCPE_BARE_STATUS (sim_set_timers (0, "CALIB=0")),
                      SCPE_ARG);
    assert_int_equal (sim_set_timers (0, "NOCATCHUP,CALIB=50"), SCPE_OK);

    stream = tmpfile ();
    assert_non_null (stream);
    assert_int_equal (sim_timer_show_catchup (stream, NULL, 0, NULL),
                      SCPE_OK);
    text = sim_timer_read_stream_text (stream);
    assert_non_null (strstr (text, "Calibrated Ticks"));
    assert_null (strstr (text, "with Catchup Ticks"));
    free (text);
    fclose (stream);

    assert_int_equal (sim_set_timers (0, "CATCHUP,CALIB=ALWAYS"), SCPE_OK);
    stream = tmpfile ();
    assert_non_null (stream);
    assert_int_equal (sim_timer_show_catchup (stream, NULL, 0, NULL),
                      SCPE_OK);
    text = sim_timer_read_stream_text (stream);
    assert_non_null (strstr (text, "with Catchup Ticks"));
    free (text);
    fclose (stream);

    stream = tmpfile ();
    assert_non_null (stream);
    assert_int_equal (sim_show_idle (stream, NULL, 0, NULL), SCPE_OK);
    text = sim_timer_read_stream_text (stream);
    assert_non_null (strstr (text, "idle disabled"));
    free (text);
    fclose (stream);

    assert_int_equal (sim_set_idle (NULL, 0, "2", NULL), SCPE_OK);
    stream = tmpfile ();
    assert_non_null (stream);
    assert_int_equal (sim_show_idle (stream, NULL, 0, NULL), SCPE_OK);
    text = sim_timer_read_stream_text (stream);
    assert_non_null (strstr (text, "idle enabled"));
    free (text);
    fclose (stream);
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
            test_sim_timer_init_measures_host_timing_with_hooks,
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
            test_sim_rtcn_get_time_and_sim_get_time_use_clock_wrapper,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_rtcn_init_rejects_invalid_timer_numbers,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_rtcn_init_unit_ticks_records_clock_state,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_rtc_legacy_wrappers_use_timer_zero,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_rtcn_calb_counts_ticks_before_calibration,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_rtcn_calb_updates_at_one_second_boundary,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_rtcn_calb_handles_backwards_host_time,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_rtcn_calb_ignores_large_host_time_gap,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_rom_read_with_delay_accepts_high_bit_words,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timer_activate_after_handles_inactive_calibrated_tick,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timer_activate_after_bounds_long_running_delay,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timer_activate_after_preserves_minimum_delay,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timer_clock_unit_activation_uses_assist_unit,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timer_activation_time_queries_standard_queue,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_clock_coschedule_orders_delta_tick_queue,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_clock_coschedule_handles_invalid_requests,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_clock_coschedule_wrappers_round_to_ticks,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_show_clock_queues_reports_coscheduled_entries,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_idle_without_calibrated_timer_counts_single_cycle,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_idle_rejects_invalid_timer_number,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_idle_sleeps_for_idle_capable_event,
            setup_sim_timer_activation_fixture,
            teardown_sim_timer_activation_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_idle_validates_stability_range,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_timer_set_and_show_helpers,
            setup_sim_timer_fixture, teardown_sim_timer_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
