#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include "sim_defs.h"

t_stat eth_test_dev_command_format(void);

static void test_eth_dev_command_formatting(void **state)
{
    (void)state;

    assert_int_equal(eth_test_dev_command_format(), SCPE_OK);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_eth_dev_command_formatting),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
