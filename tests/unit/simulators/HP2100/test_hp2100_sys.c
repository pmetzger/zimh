#include <stddef.h>

#include "test_cmocka.h"

#include "hp2100_sys_internal.h"

static void test_hold_clear_flag_is_false_when_absent(void **state)
{
    (void)state;

    assert_false(hp2100_instruction_has_clear_flag(0));
}

static void test_hold_clear_flag_ignores_other_instruction_bits(void **state)
{
    (void)state;

    assert_false(hp2100_instruction_has_clear_flag(IR_HCF - 1));
    assert_false(hp2100_instruction_has_clear_flag(~(t_value)IR_HCF));
}

static void test_hold_clear_flag_is_true_when_present(void **state)
{
    (void)state;

    assert_true(hp2100_instruction_has_clear_flag(IR_HCF));
    assert_true(hp2100_instruction_has_clear_flag(IR_HCF | 0000077u));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_hold_clear_flag_is_false_when_absent),
        cmocka_unit_test(test_hold_clear_flag_ignores_other_instruction_bits),
        cmocka_unit_test(test_hold_clear_flag_is_true_when_present),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
