#include <stddef.h>

#include "test_cmocka.h"

#include "sim_defs.h"
#include "sim_console.h"

static void test_sim_fd_isatty_rejects_invalid_fd(void **state)
{
    (void)state;

    assert_false(sim_fd_isatty(-1));
}

static void test_sim_ttisatty_returns_boolean_value(void **state)
{
    t_bool result;

    (void)state;

    result = sim_ttisatty();
    assert_true((result == TRUE) || (result == FALSE));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_sim_fd_isatty_rejects_invalid_fd),
        cmocka_unit_test(test_sim_ttisatty_returns_boolean_value),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
