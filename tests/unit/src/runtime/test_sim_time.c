#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include "sim_defs.h"
#include "sim_time.h"

static int simh_test_clock_calls = 0;
static int simh_test_sleep_calls = 0;
static int simh_test_last_clock_id = -1;
static struct timespec simh_test_last_sleep_req = {0};
static int simh_test_clock_status = 0;
static int simh_test_sleep_status = 0;
static struct timespec simh_test_clock_value = {0};
static struct timespec simh_test_sleep_rem = {0};
static int simh_test_sleep_errno = EINTR;

/* Return the configured timestamp and record the requested clock. */
static int simh_test_clock_gettime_stub(int clock_id, struct timespec *tp)
{
    ++simh_test_clock_calls;
    simh_test_last_clock_id = clock_id;
    if (tp != NULL)
        *tp = simh_test_clock_value;
    if (simh_test_clock_status != 0)
        errno = EINVAL;
    return simh_test_clock_status;
}

/* Return the configured sleep status and record the requested delay. */
static int simh_test_nanosleep_stub(const struct timespec *req,
                                    struct timespec *rem)
{
    ++simh_test_sleep_calls;
    if (req != NULL)
        simh_test_last_sleep_req = *req;
    if (rem != NULL)
        *rem = simh_test_sleep_rem;
    if (simh_test_sleep_status != 0)
        errno = simh_test_sleep_errno;
    return simh_test_sleep_status;
}

/* Reset the test hooks and per-test counters so each case is isolated. */
static int setup_sim_time_fixture(void **state)
{
    (void)state;

    simh_test_clock_calls = 0;
    simh_test_sleep_calls = 0;
    simh_test_last_clock_id = -1;
    simh_test_last_sleep_req = (struct timespec){0};
    simh_test_clock_status = 0;
    simh_test_sleep_status = 0;
    simh_test_clock_value = (struct timespec){0};
    simh_test_sleep_rem = (struct timespec){0};
    simh_test_sleep_errno = EINTR;
    sim_time_reset_test_hooks();
    errno = 0;
    return 0;
}

/* Clear any installed hooks so later tests see the real implementation. */
static int teardown_sim_time_fixture(void **state)
{
    (void)state;

    sim_time_reset_test_hooks();
    return 0;
}

/* Verify sim_clock_gettime delegates to the configured hook unchanged. */
static void test_sim_clock_gettime_uses_installed_hook(void **state)
{
    struct timespec now = {0};

    (void)state;

    simh_test_clock_value = (struct timespec){.tv_sec = 123, .tv_nsec = 456};
    sim_time_set_test_hooks(simh_test_clock_gettime_stub, NULL);

    assert_int_equal(sim_clock_gettime(CLOCK_REALTIME, &now), 0);
    assert_int_equal(simh_test_clock_calls, 1);
    assert_int_equal(simh_test_last_clock_id, CLOCK_REALTIME);
    assert_int_equal(now.tv_sec, 123);
    assert_int_equal(now.tv_nsec, 456);
}

/* Verify sim_nanosleep delegates to the configured hook unchanged. */
static void test_sim_nanosleep_uses_installed_hook(void **state)
{
    struct timespec req = {.tv_sec = 2, .tv_nsec = 300};
    struct timespec rem = {0};

    (void)state;

    simh_test_sleep_rem = (struct timespec){.tv_sec = 1, .tv_nsec = 200};
    sim_time_set_test_hooks(NULL, simh_test_nanosleep_stub);

    assert_int_equal(sim_nanosleep(&req, &rem), 0);
    assert_int_equal(simh_test_sleep_calls, 1);
    assert_int_equal(simh_test_last_sleep_req.tv_sec, 2);
    assert_int_equal(simh_test_last_sleep_req.tv_nsec, 300);
    assert_int_equal(rem.tv_sec, 1);
    assert_int_equal(rem.tv_nsec, 200);
}

/* Verify sim_nanosleep propagates hook failures and errno unchanged. */
static void test_sim_nanosleep_propagates_hook_failure(void **state)
{
    struct timespec req = {.tv_sec = 1, .tv_nsec = 500};

    (void)state;

    simh_test_sleep_status = -1;
    simh_test_sleep_errno = EBUSY;
    sim_time_set_test_hooks(NULL, simh_test_nanosleep_stub);

    errno = 0;
    assert_int_equal(sim_nanosleep(&req, NULL), -1);
    assert_int_equal(simh_test_sleep_calls, 1);
    assert_int_equal(simh_test_last_sleep_req.tv_sec, 1);
    assert_int_equal(simh_test_last_sleep_req.tv_nsec, 500);
    assert_int_equal(errno, EBUSY);
}

/* Verify sim_time reads CLOCK_REALTIME seconds through sim_clock_gettime. */
static void test_sim_time_returns_seconds_and_stores_output(void **state)
{
    time_t now = 0;

    (void)state;

    simh_test_clock_value = (struct timespec){.tv_sec = 789, .tv_nsec = 10};
    sim_time_set_test_hooks(simh_test_clock_gettime_stub, NULL);

    assert_int_equal(sim_time(&now), 789);
    assert_int_equal(now, 789);
    assert_int_equal(simh_test_last_clock_id, CLOCK_REALTIME);
}

/* Verify sim_time reports failure as -1 and propagates it to the caller. */
static void test_sim_time_reports_clock_failure(void **state)
{
    time_t now = 5;

    (void)state;

    simh_test_clock_status = -1;
    sim_time_set_test_hooks(simh_test_clock_gettime_stub, NULL);

    assert_int_equal(sim_time(&now), (time_t)-1);
    assert_int_equal(now, (time_t)-1);
}

/* Verify sim_time also works when the optional output pointer is NULL. */
static void test_sim_time_accepts_null_output_pointer(void **state)
{
    (void)state;

    simh_test_clock_value = (struct timespec){.tv_sec = 321, .tv_nsec = 99};
    sim_time_set_test_hooks(simh_test_clock_gettime_stub, NULL);

    assert_int_equal(sim_time(NULL), 321);
}

/* Verify reset restores the real clock and sleep wrappers. */
static void test_sim_time_reset_restores_default_wrappers(void **state)
{
    struct timespec now = {0};
    struct timespec req = {0};
    time_t sec = 0;

    (void)state;

    sim_time_set_test_hooks(simh_test_clock_gettime_stub,
                            simh_test_nanosleep_stub);
    sim_time_reset_test_hooks();

    assert_int_equal(sim_clock_gettime(CLOCK_REALTIME, &now), 0);
    assert_true(now.tv_sec > 0);
    assert_int_equal(sim_time(&sec), sec);
    assert_true(sec > 0);
    assert_int_equal(sim_nanosleep(&req, NULL), 0);
}

/* Verify sim_sleep turns whole seconds into one nanosleep request. */
static void test_sim_sleep_converts_seconds_to_timespec(void **state)
{
    (void)state;

    sim_time_set_test_hooks(NULL, simh_test_nanosleep_stub);

    sim_sleep(7);
    assert_int_equal(simh_test_sleep_calls, 1);
    assert_int_equal(simh_test_last_sleep_req.tv_sec, 7);
    assert_int_equal(simh_test_last_sleep_req.tv_nsec, 0);
}

/* CMocka-aware nanosleep stub for exercising EINTR retry behavior. */
static int simh_test_nanosleep_eintr_then_success(const struct timespec *req,
                                                  struct timespec *rem)
{
    function_called();
    check_expected_ptr(req);
    check_expected_ptr(rem);
    ++simh_test_sleep_calls;
    simh_test_last_sleep_req = *req;
    if (simh_test_sleep_calls == 1) {
        errno = EINTR;
        if (rem != NULL)
            *rem = simh_test_sleep_rem;
        return -1;
    }
    return 0;
}

/* Verify sim_sleep retries interrupted sleeps using the remaining time. */
static void test_sim_sleep_uses_remaining_time_after_eintr(void **state)
{
    (void)state;

    simh_test_sleep_rem = (struct timespec){.tv_sec = 1, .tv_nsec = 200};
    sim_time_set_test_hooks(NULL, simh_test_nanosleep_eintr_then_success);

    expect_function_call(simh_test_nanosleep_eintr_then_success);
    expect_any(simh_test_nanosleep_eintr_then_success, req);
    expect_any(simh_test_nanosleep_eintr_then_success, rem);
    expect_function_call(simh_test_nanosleep_eintr_then_success);
    expect_any(simh_test_nanosleep_eintr_then_success, req);
    expect_any(simh_test_nanosleep_eintr_then_success, rem);

    sim_sleep(5);
    assert_int_equal(simh_test_sleep_calls, 2);
    assert_int_equal(simh_test_last_sleep_req.tv_sec, 1);
    assert_int_equal(simh_test_last_sleep_req.tv_nsec, 200);
}

/* Verify sim_sleep stops retrying on non-EINTR errors. */
static void test_sim_sleep_stops_on_non_eintr_error(void **state)
{
    (void)state;

    simh_test_sleep_status = -1;
    simh_test_sleep_errno = EIO;
    sim_time_set_test_hooks(NULL, simh_test_nanosleep_stub);

    sim_sleep(3);
    assert_int_equal(simh_test_sleep_calls, 1);
    assert_int_equal(simh_test_last_sleep_req.tv_sec, 3);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_sim_clock_gettime_uses_installed_hook, setup_sim_time_fixture,
            teardown_sim_time_fixture),
        cmocka_unit_test_setup_teardown(test_sim_nanosleep_uses_installed_hook,
                                        setup_sim_time_fixture,
                                        teardown_sim_time_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_nanosleep_propagates_hook_failure,
            setup_sim_time_fixture, teardown_sim_time_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_time_returns_seconds_and_stores_output,
            setup_sim_time_fixture, teardown_sim_time_fixture),
        cmocka_unit_test_setup_teardown(test_sim_time_reports_clock_failure,
                                        setup_sim_time_fixture,
                                        teardown_sim_time_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_time_accepts_null_output_pointer, setup_sim_time_fixture,
            teardown_sim_time_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_time_reset_restores_default_wrappers,
            setup_sim_time_fixture, teardown_sim_time_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sleep_converts_seconds_to_timespec, setup_sim_time_fixture,
            teardown_sim_time_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sleep_uses_remaining_time_after_eintr,
            setup_sim_time_fixture, teardown_sim_time_fixture),
        cmocka_unit_test_setup_teardown(test_sim_sleep_stops_on_non_eintr_error,
                                        setup_sim_time_fixture,
                                        teardown_sim_time_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
