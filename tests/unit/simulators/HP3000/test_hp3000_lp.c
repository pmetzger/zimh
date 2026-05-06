#include <stddef.h>
#include <stdbool.h>

#include "test_cmocka.h"

#include "hp3000_lp_internal.h"

static void test_device_command_out_uses_normal_polarity(void **state)
{
    (void)state;

    assert_false(hp3000_lp_device_command_out(CLEAR, false));
    assert_true(hp3000_lp_device_command_out(SET, false));
}

static void test_device_command_out_uses_inverted_polarity(void **state)
{
    (void)state;

    assert_true(hp3000_lp_device_command_out(CLEAR, true));
    assert_false(hp3000_lp_device_command_out(SET, true));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_device_command_out_uses_normal_polarity),
        cmocka_unit_test(test_device_command_out_uses_inverted_polarity),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
