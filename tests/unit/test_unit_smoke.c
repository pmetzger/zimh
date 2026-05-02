#include <stddef.h>
#include <string.h>
#include <sys/stat.h>

#include "test_cmocka.h"

#include "test_support.h"

/* Verify that the injected source and binary roots are usable paths. */
static void test_support_roots_exist(void **state)
{
    const char *source_root = simh_test_source_root();
    const char *binary_root = simh_test_binary_root();
    struct stat st;

    (void)state;

    assert_non_null(source_root);
    assert_true(strlen(source_root) > 0);
    assert_int_equal(stat(source_root, &st), 0);
    assert_true(S_ISDIR(st.st_mode));

    assert_non_null(binary_root);
    assert_true(strlen(binary_root) > 0);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_support_roots_exist),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
