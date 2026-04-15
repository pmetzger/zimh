#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <cmocka.h>

#include "test_support.h"

static void test_support_reads_fixture(void **state)
{
    static const char fixture_text[] = "sample fixture for unit tests\n";
    char path[1024];
    void *data;
    size_t size;

    (void)state;

    assert_int_equal(
        simh_test_fixture_path(path, sizeof(path), "sample.txt"), 0);
    assert_true(strstr(path, "tests/unit/fixtures/sample.txt") != NULL);

    assert_int_equal(simh_test_read_fixture("sample.txt", &data, &size), 0);
    assert_int_equal(size, strlen(fixture_text));
    assert_string_equal((const char *)data, fixture_text);

    free(data);
}

static void test_support_temp_dir_round_trip(void **state)
{
    char dir_path[1024];
    char fixture_path[1024];
    char output_path[1024];
    void *data;
    size_t size;
    struct stat st;
    int length;

    (void)state;

    assert_int_equal(
        simh_test_make_temp_dir(dir_path, sizeof(dir_path), "simh-unit"), 0);
    assert_int_equal(stat(dir_path, &st), 0);
    assert_true(S_ISDIR(st.st_mode));

    assert_int_equal(
        simh_test_fixture_path(fixture_path, sizeof(fixture_path),
                               "sample.txt"),
        0);
    assert_int_equal(simh_test_read_fixture("sample.txt", &data, &size), 0);

    length = snprintf(output_path, sizeof(output_path), "%s/%s", dir_path,
                      "copy.txt");
    assert_true(length > 0);
    assert_true((size_t)length < sizeof(output_path));
    assert_int_equal(simh_test_write_file(output_path, data, size), 0);
    assert_int_equal(simh_test_files_equal(fixture_path, output_path), 1);

    free(data);

    assert_int_equal(simh_test_remove_path(dir_path), 0);
    assert_int_not_equal(stat(dir_path, &st), 0);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_support_reads_fixture),
        cmocka_unit_test(test_support_temp_dir_round_trip),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
