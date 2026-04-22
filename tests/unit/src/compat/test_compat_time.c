#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include "sim_compat.h"

/* Verify localtime_r returns the destination pointer and matches the
   documented broken-down local time for a stable epoch value. */
static void test_localtime_r_returns_result_and_breaks_down_time(void **state)
{
    time_t when = (time_t)0;
    struct tm result = {0};
    struct tm *tmnow;

    (void)state;

    tmnow = localtime_r(&when, &result);
    assert_ptr_equal(tmnow, &result);
    assert_int_equal(result.tm_year, 70);
    assert_int_equal(result.tm_mon, 0);
    assert_int_equal(result.tm_mday, 1);
    assert_int_equal(result.tm_hour, 0);
    assert_int_equal(result.tm_min, 0);
    assert_int_equal(result.tm_sec, 0);
}

/* Verify gmtime_r returns the destination pointer and breaks down the epoch
   in UTC. */
static void test_gmtime_r_returns_result_and_breaks_down_time(void **state)
{
    time_t when = (time_t)0;
    struct tm result = {0};
    struct tm *tmnow;

    (void)state;

    tmnow = gmtime_r(&when, &result);
    assert_ptr_equal(tmnow, &result);
    assert_int_equal(result.tm_year, 70);
    assert_int_equal(result.tm_mon, 0);
    assert_int_equal(result.tm_mday, 1);
    assert_int_equal(result.tm_hour, 0);
    assert_int_equal(result.tm_min, 0);
    assert_int_equal(result.tm_sec, 0);
}

/* Verify both shims fail closed when passed NULL arguments. */
static void test_compat_time_shims_reject_null_arguments(void **state)
{
    time_t when = (time_t)0;
    struct tm result = {0};

    (void)state;

    assert_null(localtime_r(NULL, &result));
    assert_null(localtime_r(&when, NULL));
    assert_null(gmtime_r(NULL, &result));
    assert_null(gmtime_r(&when, NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(
            test_localtime_r_returns_result_and_breaks_down_time),
        cmocka_unit_test(
            test_gmtime_r_returns_result_and_breaks_down_time),
        cmocka_unit_test(test_compat_time_shims_reject_null_arguments),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
