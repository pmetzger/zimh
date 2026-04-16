#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "sim_defs.h"
#include "sim_fio.h"
#include "test_support.h"

struct sim_fio_fixture {
    char temp_dir[1024];
    char nested_dir[1024];
    char file_path[1024];
    char normalized_path[1024];
    char path_only[1024];
};

static int setup_sim_fio_fixture(void **state)
{
    static const uint8_t file_bytes[] = {0x10, 0x20, 0x30, 0x40, 0x50};
    struct sim_fio_fixture *fixture;

    fixture = calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    assert_int_equal(simh_test_make_temp_dir(fixture->temp_dir,
                                             sizeof(fixture->temp_dir),
                                             "sim-fio"),
                     0);
    assert_int_equal(simh_test_join_path(fixture->nested_dir,
                                         sizeof(fixture->nested_dir),
                                         fixture->temp_dir, "alpha"),
                     0);
    assert_int_equal(sim_mkdir(fixture->nested_dir), 0);
    assert_int_equal(simh_test_join_path(fixture->file_path,
                                         sizeof(fixture->file_path),
                                         fixture->nested_dir, "report.bin"),
                     0);
    assert_int_equal(simh_test_write_file(fixture->file_path, file_bytes,
                                          sizeof(file_bytes)),
                     0);

    assert_int_equal(snprintf(fixture->normalized_path,
                              sizeof(fixture->normalized_path),
                              "%s/alpha/report.bin", fixture->temp_dir) <
                         (int)sizeof(fixture->normalized_path),
                     1);
    assert_int_equal(snprintf(fixture->path_only, sizeof(fixture->path_only),
                              "%s/alpha/", fixture->temp_dir) <
                         (int)sizeof(fixture->path_only),
                     1);

    *state = fixture;
    return 0;
}

static int teardown_sim_fio_fixture(void **state)
{
    struct sim_fio_fixture *fixture = *state;

    assert_int_equal(simh_test_remove_path(fixture->temp_dir), 0);
    free(fixture);
    *state = NULL;
    return 0;
}

/* Verify trailing whitespace is removed in place and the input pointer
   is preserved. */
static void test_sim_trim_endspc_strips_trailing_whitespace(void **state)
{
    char text[] = "alpha beta \t \n";
    char *result;

    (void)state;

    result = sim_trim_endspc(text);
    assert_ptr_equal(result, text);
    assert_string_equal(text, "alpha beta");
}

/* Verify the portable strlcpy/strlcat helpers report truncation like
   their BSD counterparts. */
static void test_sim_strlcpy_and_strlcat_follow_bsd_semantics(void **state)
{
    char copy_buf[5];
    char cat_buf[7] = "abc";

    (void)state;

    assert_int_equal(sim_strlcpy(copy_buf, "sample", sizeof(copy_buf)), 6);
    assert_string_equal(copy_buf, "samp");

    assert_int_equal(sim_strlcat(cat_buf, "defghi", sizeof(cat_buf)), 9);
    assert_string_equal(cat_buf, "abcdef");
}

/* Verify the case-folding helpers preserve ordering and optionally
   collapse whitespace runs. */
static void
test_sim_string_compare_helpers_handle_case_and_whitespace(void **state)
{
    (void)state;

    assert_int_equal(sim_strncasecmp("Alpha", "aLpHaZ", 5), 0);
    assert_true(sim_strncasecmp("Alpha", "AlpHz", 5) < 0);
    assert_true(sim_strcasecmp("beta", "ALPHA") > 0);
    assert_int_equal(sim_strcasecmp("Gamma", "gAmMa"), 0);

    assert_int_equal(sim_strwhitecasecmp("Alpha\t beta", "alpha beta", TRUE),
                     0);
    assert_true(sim_strwhitecasecmp("Alpha Beta", "alpha beta", FALSE) < 0);
}

/* Verify filepath extraction normalizes paths and returns full, path,
   name, and extension segments. */
static void
test_sim_filepath_parts_splits_normalized_path_components(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    char request_path[1024];
    char *full_path;
    char *path_only;
    char *name_only;
    char *ext_only;
    char *name_ext;

    assert_int_equal(snprintf(request_path, sizeof(request_path),
                              "%s//alpha/./beta/../%s", fixture->temp_dir,
                              "report.bin") < (int)sizeof(request_path),
                     1);

    full_path = sim_filepath_parts(request_path, "f");
    path_only = sim_filepath_parts(request_path, "p");
    name_only = sim_filepath_parts(request_path, "n");
    ext_only = sim_filepath_parts(request_path, "x");
    name_ext = sim_filepath_parts(request_path, "nx");

    assert_non_null(full_path);
    assert_non_null(path_only);
    assert_non_null(name_only);
    assert_non_null(ext_only);
    assert_non_null(name_ext);

    assert_string_equal(full_path, fixture->normalized_path);
    assert_string_equal(path_only, fixture->path_only);
    assert_string_equal(name_only, "report");
    assert_string_equal(ext_only, ".bin");
    assert_string_equal(name_ext, "report.bin");

    free(full_path);
    free(path_only);
    free(name_only);
    free(ext_only);
    free(name_ext);
}

/* Verify filesize extraction reports the stat-backed size for an
   existing file. */
static void test_sim_filepath_parts_reports_existing_file_size(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    char request_path[1024];
    char *size_text;

    assert_int_equal(snprintf(request_path, sizeof(request_path),
                              "%s/alpha/report.bin",
                              fixture->temp_dir) < (int)sizeof(request_path),
                     1);

    size_text = sim_filepath_parts(request_path, "z");
    assert_non_null(size_text);
    assert_string_equal(size_text, "5 ");
    free(size_text);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_sim_trim_endspc_strips_trailing_whitespace),
        cmocka_unit_test(test_sim_strlcpy_and_strlcat_follow_bsd_semantics),
        cmocka_unit_test(
            test_sim_string_compare_helpers_handle_case_and_whitespace),
        cmocka_unit_test_setup_teardown(
            test_sim_filepath_parts_splits_normalized_path_components,
            setup_sim_fio_fixture, teardown_sim_fio_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_filepath_parts_reports_existing_file_size,
            setup_sim_fio_fixture, teardown_sim_fio_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
