#include "test_cmocka.h"

#include "3b2_mau.h"

static void test_single_precision_sign_is_a_predicate(void **state)
{
    (void)state;

    assert_int_equal(SFP_SIGN(0x00000000u), 0);
    assert_int_equal(SFP_SIGN(0x7fffffffu), 0);
    assert_int_equal(SFP_SIGN(0x80000000u), 1);
    assert_int_equal(SFP_SIGN(0xffffffffu), 1);
}

static void test_double_precision_sign_is_a_predicate(void **state)
{
    (void)state;

    assert_int_equal(DFP_SIGN(0x0000000000000000ull), 0);
    assert_int_equal(DFP_SIGN(0x7fffffffffffffffull), 0);
    assert_int_equal(DFP_SIGN(0x8000000000000000ull), 1);
    assert_int_equal(DFP_SIGN(0xffffffffffffffffull), 1);
}

static void test_extended_precision_sign_is_a_predicate(void **state)
{
    XFP positive = {0x00000000u, 0, 0};
    XFP negative = {0x00008000u, 0, 0};
    XFP high_bits_ignored = {0xffff7fffu, 0, 0};
    XFP high_bits_with_sign = {0xffff8000u, 0, 0};

    (void)state;

    assert_int_equal(XFP_SIGN(&positive), 0);
    assert_int_equal(XFP_SIGN(&negative), 1);
    assert_int_equal(XFP_SIGN(&high_bits_ignored), 0);
    assert_int_equal(XFP_SIGN(&high_bits_with_sign), 1);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_single_precision_sign_is_a_predicate),
        cmocka_unit_test(test_double_precision_sign_is_a_predicate),
        cmocka_unit_test(test_extended_precision_sign_is_a_predicate),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
