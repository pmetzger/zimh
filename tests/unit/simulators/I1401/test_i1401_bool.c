#include <stdbool.h>
#include <stddef.h>

#include "test_cmocka.h"

#include "i1401_bool_internal.h"

static void test_set_modifier_value_is_a_predicate(void **state)
{
    (void)state;

    assert_false(i1401_set_modifier_enabled(0));
    assert_true(i1401_set_modifier_enabled(1));
    assert_true(i1401_set_modifier_enabled(2));
    assert_true(i1401_set_modifier_enabled(-1));
}

static void test_mask_value_is_a_predicate(void **state)
{
    (void)state;

    assert_false(i1401_mask_is_set(0, 010));
    assert_false(i1401_mask_is_set(020, 010));
    assert_true(i1401_mask_is_set(030, 010));
}

static void test_fortran_conversion_requires_f_switch(void **state)
{
    (void)state;

    assert_false(i1401_fortran_conversion_switch_requested(0));
    assert_false(i1401_fortran_conversion_switch_requested(SWMASK('M')));
    assert_true(i1401_fortran_conversion_switch_requested(SWMASK('F')));
    assert_true(
        i1401_fortran_conversion_switch_requested(SWMASK('F') | SWMASK('M')));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_set_modifier_value_is_a_predicate),
        cmocka_unit_test(test_mask_value_is_a_predicate),
        cmocka_unit_test(test_fortran_conversion_requires_f_switch),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
