#include <math.h>

#include "test_cmocka.h"

#include "sel32_clk_internal.h"

/* Verify interval-timer usec conversion preserves ordinary count values. */
static void test_itm_remaining_counts_converts_usecs_to_counts(void **state)
{
    (void)state;

    assert_int_equal(sel32_itm_remaining_counts(0.0, 3840), 0);
    assert_int_equal(sel32_itm_remaining_counts(38.4, 3840), 1);
    assert_int_equal(sel32_itm_remaining_counts(76.8, 3840), 2);
    assert_int_equal(sel32_itm_remaining_counts(95.9, 3840), 2);
    assert_int_equal(sel32_itm_remaining_counts(96.0, 3840), 2);
}

/* Verify interval-timer count reads wrap to the 32-bit timer register. */
static void test_itm_remaining_counts_wraps_to_32_bits(void **state)
{
    const double one_count_usecs = 1.0;
    const double wrap_usecs = 4294967296.0 * one_count_usecs;

    (void)state;

    assert_int_equal(
        sel32_itm_remaining_counts(wrap_usecs - one_count_usecs, 100),
        0xffffffffu);
    assert_int_equal(sel32_itm_remaining_counts(wrap_usecs, 100), 0);
    assert_int_equal(
        sel32_itm_remaining_counts(wrap_usecs + one_count_usecs, 100), 1);
}

/* Verify invalid or non-positive inputs do not produce host conversion UB. */
static void test_itm_remaining_counts_rejects_non_positive_inputs(void **state)
{
    (void)state;

    assert_int_equal(sel32_itm_remaining_counts(-1.0, 3840), 0);
    assert_int_equal(sel32_itm_remaining_counts(INFINITY, 3840), 0);
    assert_int_equal(sel32_itm_remaining_counts(NAN, 3840), 0);
    assert_int_equal(sel32_itm_remaining_counts(100.0, 0), 0);
    assert_int_equal(sel32_itm_remaining_counts(100.0, -3840), 0);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_itm_remaining_counts_converts_usecs_to_counts),
        cmocka_unit_test(test_itm_remaining_counts_wraps_to_32_bits),
        cmocka_unit_test(test_itm_remaining_counts_rejects_non_positive_inputs),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
