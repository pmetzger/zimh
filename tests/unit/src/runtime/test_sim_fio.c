#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <cmocka.h>

#include "sim_defs.h"
#include "sim_fio.h"
#include "test_support.h"

struct sim_fio_fixture {
    char temp_dir[1024];
    char nested_dir[1024];
    char file_path[1024];
    char copy_path[1024];
    char normalized_path[1024];
    char path_only[1024];
    char original_cwd[1024];
};

struct filelist_context {
    char **entries;
    size_t count;
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
    assert_int_equal(simh_test_join_path(fixture->copy_path,
                                         sizeof(fixture->copy_path),
                                         fixture->nested_dir, "copy.bin"),
                     0);
    assert_int_equal(simh_test_write_file(fixture->file_path, file_bytes,
                                          sizeof(file_bytes)),
                     0);
    assert_non_null(
        sim_getcwd(fixture->original_cwd, sizeof(fixture->original_cwd)));

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

static void filelist_callback(const char *directory, const char *filename,
                              t_offset file_size, const struct stat *filestat,
                              void *context)
{
    struct filelist_context *result = context;
    char full_path[1024];
    char **entries;

    (void)file_size;
    (void)filestat;

    assert_int_equal(snprintf(full_path, sizeof(full_path), "%s%s", directory,
                              filename) < (int)sizeof(full_path),
                     1);

    entries = realloc(result->entries, (result->count + 1) * sizeof(*entries));
    assert_non_null(entries);
    result->entries = entries;
    result->entries[result->count] = strdup(full_path);
    assert_non_null(result->entries[result->count]);
    ++result->count;
}

static void free_filelist_context(struct filelist_context *context)
{
    size_t i;

    for (i = 0; i < context->count; ++i) {
        free(context->entries[i]);
    }
    free(context->entries);
    context->entries = NULL;
    context->count = 0;
}

static int filelist_contains(char **filelist, const char *path)
{
    size_t i;

    if (filelist == NULL) {
        return 0;
    }

    for (i = 0; filelist[i] != NULL; ++i) {
        if (strcmp(filelist[i], path) == 0) {
            return 1;
        }
    }
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

/* Verify filepath extraction handles empty-part requests and timestamp
   formatting after quoted home-directory expansion. */
static void
test_sim_filepath_parts_handles_home_expansion_and_time_fields(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    char *saved_home;
    char quoted_home_file[] = "\"~/alpha/report.bin\"";
    char *stripped_path;
    char *time_text;

    saved_home = getenv("HOME");
    assert_int_equal(setenv("HOME", fixture->temp_dir, 1), 0);

    stripped_path = sim_filepath_parts(quoted_home_file, "");
    time_text = sim_filepath_parts(quoted_home_file, "t");
    assert_non_null(stripped_path);
    assert_non_null(time_text);
    assert_string_equal(stripped_path, fixture->normalized_path);
    assert_non_null(strstr(time_text, "/"));
    assert_non_null(strstr(time_text, "M "));

    free(time_text);
    free(stripped_path);

    if (saved_home != NULL) {
        assert_int_equal(setenv("HOME", saved_home, 1), 0);
    } else {
        assert_int_equal(unsetenv("HOME"), 0);
    }
}

/* Verify byte-swap helpers reverse multi-byte objects when the host is
   treated as big-endian. */
static void test_sim_byte_swap_helpers_reverse_multibyte_elements(void **state)
{
    uint16_t buf16[] = {0x1234, 0xABCD};
    uint8_t src32[] = {0x10, 0x20, 0x30, 0x40, 0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t dst32[sizeof(src32)];
    t_bool saved_sim_end;

    (void)state;

    saved_sim_end = sim_end;
    sim_end = FALSE;

    sim_byte_swap_data(buf16, sizeof(buf16[0]), 2);
    assert_int_equal(buf16[0], 0x3412);
    assert_int_equal(buf16[1], 0xCDAB);

    memset(dst32, 0, sizeof(dst32));
    sim_buf_copy_swapped(dst32, src32, 4, 2);
    assert_memory_equal(
        dst32, ((uint8_t[]){0x40, 0x30, 0x20, 0x10, 0xDD, 0xCC, 0xBB, 0xAA}),
        sizeof(dst32));

    sim_end = saved_sim_end;
}

/* Verify endian-aware fread/fwrite preserve logical values and that
   sim_fsize_ex keeps the caller's file position intact. */
static void
test_sim_fread_and_fwrite_round_trip_in_big_endian_mode(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    uint16_t words_out[] = {0x1122, 0x3344, 0x5566};
    uint16_t words_in[3] = {0, 0, 0};
    uint8_t raw_bytes[sizeof(words_out)];
    FILE *file;
    t_bool saved_sim_end;

    saved_sim_end = sim_end;
    sim_end = FALSE;

    file = sim_fopen(fixture->copy_path, "wb+");
    assert_non_null(file);

    assert_int_equal(sim_fwrite(words_out, sizeof(words_out[0]), 3, file), 3);
    assert_int_equal(sim_fsize_ex(file), (t_offset)sizeof(words_out));
    assert_int_equal(sim_ftell(file), (t_offset)sizeof(words_out));

    assert_int_equal(sim_fseeko(file, 0, SEEK_SET), 0);
    assert_int_equal(fread(raw_bytes, 1, sizeof(raw_bytes), file),
                     sizeof(raw_bytes));
    assert_memory_equal(raw_bytes,
                        ((uint8_t[]){0x11, 0x22, 0x33, 0x44, 0x55, 0x66}),
                        sizeof(raw_bytes));

    assert_int_equal(sim_fseeko(file, sizeof(words_out[0]), SEEK_SET), 0);
    assert_int_equal(sim_fsize_ex(file), (t_offset)sizeof(words_out));
    assert_int_equal(sim_ftell(file), (t_offset)sizeof(words_out[0]));

    assert_int_equal(sim_fseeko(file, 0, SEEK_SET), 0);
    assert_int_equal(sim_fread(words_in, sizeof(words_in[0]), 3, file), 3);
    assert_memory_equal(words_in, words_out, sizeof(words_out));

    fclose(file);
    sim_end = saved_sim_end;
}

/* Verify sim_fwrite advances correctly across multiple internal flip
   buffers instead of skipping to the wrong source chunk. */
static void test_sim_fwrite_handles_multiple_flip_buffers(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    const size_t count = (FLIP_SIZE / sizeof(uint16_t)) + 2;
    uint16_t *words_out;
    uint16_t *words_in;
    FILE *file;
    t_bool saved_sim_end;
    size_t i;

    words_out = calloc(count + 2, sizeof(*words_out));
    words_in = calloc(count, sizeof(*words_in));
    assert_non_null(words_out);
    assert_non_null(words_in);

    for (i = 0; i < count; ++i) {
        words_out[i] = (uint16_t)i;
    }
    words_out[count] = 0xDEAD;
    words_out[count + 1] = 0xBEEF;

    saved_sim_end = sim_end;
    sim_end = FALSE;

    file = sim_fopen(fixture->copy_path, "wb+");
    assert_non_null(file);

    assert_int_equal(sim_fwrite(words_out, sizeof(words_out[0]), count, file),
                     count);
    assert_int_equal(sim_fseeko(file, 0, SEEK_SET), 0);
    assert_int_equal(sim_fread(words_in, sizeof(words_in[0]), count, file),
                     count);

    for (i = 0; i < count; ++i) {
        assert_int_equal(words_in[i], words_out[i]);
    }

    fclose(file);
    sim_end = saved_sim_end;
    free(words_in);
    free(words_out);
}

/* Verify file-size and seekability helpers behave on real files and
   named-path queries. */
static void test_sim_file_size_helpers_report_sizes_consistently(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    FILE *file;

    file = sim_fopen(fixture->file_path, "rb");
    assert_non_null(file);

    assert_true(sim_can_seek(file));
    assert_int_equal(sim_fsize(file), 5);
    assert_int_equal(sim_fsize_ex(file), 5);
    assert_int_equal(sim_fsize_name(fixture->file_path), 5);
    assert_int_equal(sim_fsize_name_ex(fixture->file_path), 5);
    assert_int_equal(sim_fsize_name_ex(fixture->copy_path), 0);

    fclose(file);
}

/* Verify sim_fopen and sim_set_fsize can create and resize a file
   addressed through home-directory expansion. */
static void test_sim_fopen_and_sim_set_fsize_handle_home_expansion(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    char *saved_home;
    char relative_file[] = "~/resize.bin";
    char quoted_file[] = "\"~/resize.bin\"";
    FILE *file;
    struct stat statb;

    saved_home = getenv("HOME");
    assert_int_equal(setenv("HOME", fixture->temp_dir, 1), 0);

    file = sim_fopen(relative_file, "wb+");
    assert_non_null(file);
    assert_int_equal(fwrite("abcdef", 1, 6, file), 6);
    assert_int_equal(fflush(file), 0);
    assert_int_equal(sim_set_fsize(file, 3), 0);
    fclose(file);

    assert_int_equal(sim_stat(quoted_file, &statb), 0);
    assert_int_equal((int)statb.st_size, 3);

    if (saved_home != NULL) {
        assert_int_equal(setenv("HOME", saved_home, 1), 0);
    } else {
        assert_int_equal(unsetenv("HOME"), 0);
    }
}

/* Verify sim_set_file_times updates the file timestamps to caller-
   supplied values. */
static void test_sim_set_file_times_updates_stat_times(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    struct stat statb;
    time_t access_time = (time_t)1700000000;
    time_t write_time = (time_t)1700000123;

    assert_int_equal(
        sim_set_file_times(fixture->file_path, access_time, write_time),
        SCPE_OK);
    assert_int_equal(sim_stat(fixture->file_path, &statb), 0);
    assert_int_equal(statb.st_atime, access_time);
    assert_int_equal(statb.st_mtime, write_time);
}

/* Verify sim_set_fifo_nonblock distinguishes FIFOs from regular files
   and sets O_NONBLOCK on a FIFO stream. */
static void test_sim_set_fifo_nonblock_marks_fifo_streams(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    char fifo_path[1024];
    FILE *file;
    int fd;
    int flags;

    assert_int_equal(simh_test_join_path(fifo_path, sizeof(fifo_path),
                                         fixture->temp_dir, "data.fifo"),
                     0);
    assert_int_equal(mkfifo(fifo_path, 0600), 0);

    fd = open(fifo_path, O_RDWR);
    assert_true(fd >= 0);
    file = fdopen(fd, "r+");
    assert_non_null(file);
    assert_int_equal(sim_set_fifo_nonblock(file), 0);
    flags = fcntl(fileno(file), F_GETFL, 0);
    assert_true(flags >= 0);
    assert_true((flags & O_NONBLOCK) != 0);
    fclose(file);

    file = sim_fopen(fixture->file_path, "rb");
    assert_non_null(file);
    assert_int_equal(sim_set_fifo_nonblock(file), -1);
    fclose(file);
}

/* Verify directory scanning, file-list collection, and file-copying all
   operate on the same discovered file set. */
static void
test_sim_dir_scan_get_filelist_and_copyfile_work_together(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    struct filelist_context context = {0};
    char pattern[1024];
    char second_file[1024];
    char **filelist;
    struct stat statb;

    assert_int_equal(simh_test_join_path(second_file, sizeof(second_file),
                                         fixture->nested_dir, "notes.txt"),
                     0);
    assert_int_equal(simh_test_write_file(second_file, "abc", 3), 0);

    assert_int_equal(sim_copyfile(fixture->file_path, fixture->copy_path, TRUE),
                     SCPE_OK);
    assert_int_equal(
        simh_test_files_equal(fixture->file_path, fixture->copy_path), 1);

    assert_int_equal(snprintf(pattern, sizeof(pattern), "%s/*",
                              fixture->nested_dir) < (int)sizeof(pattern),
                     1);
    assert_int_equal(sim_dir_scan(pattern, filelist_callback, &context),
                     SCPE_OK);
    assert_int_equal(context.count, 3);

    filelist = sim_get_filelist(pattern);
    assert_non_null(filelist);
    assert_true(filelist_contains(filelist, fixture->file_path));
    assert_true(filelist_contains(filelist, fixture->copy_path));
    assert_true(filelist_contains(filelist, second_file));

    assert_int_equal(sim_stat(fixture->copy_path, &statb), 0);
    assert_int_equal((int)statb.st_size, 5);

    sim_free_filelist(&filelist);
    assert_null(filelist);
    free_filelist_context(&context);
}

/* Verify sim_dir_scan reports an argument-style failure when a pattern
   does not match any files. */
static void test_sim_dir_scan_reports_missing_matches(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    char pattern[1024];

    assert_int_equal(snprintf(pattern, sizeof(pattern), "%s/*.missing",
                              fixture->nested_dir) < (int)sizeof(pattern),
                     1);
    assert_int_equal(sim_dir_scan(pattern, filelist_callback, NULL), SCPE_ARG);
}

/* Verify the directory wrappers and home-directory expansion can be used
   to create, enter, inspect, and remove a temporary subdirectory. */
static void test_sim_directory_wrappers_handle_home_expansion(void **state)
{
    struct sim_fio_fixture *fixture = *state;
    char *saved_home;
    char relative_home_dir[] = "~/simh-fio-home";
    char quoted_home_dir[] = "\"~/simh-fio-home\"";
    char expanded_home_dir[1024];
    char cwd_buf[1024];
    struct stat statb;
    struct stat cwd_statb;

    saved_home = getenv("HOME");
    assert_int_equal(setenv("HOME", fixture->temp_dir, 1), 0);
    assert_int_equal(snprintf(expanded_home_dir, sizeof(expanded_home_dir),
                              "%s/simh-fio-home", fixture->temp_dir) <
                         (int)sizeof(expanded_home_dir),
                     1);

    assert_int_equal(sim_mkdir(relative_home_dir), 0);
    assert_int_equal(sim_stat(quoted_home_dir, &statb), 0);
    assert_true(S_ISDIR(statb.st_mode));

    assert_int_equal(sim_chdir(relative_home_dir), 0);
    assert_non_null(sim_getcwd(cwd_buf, sizeof(cwd_buf)));
    assert_non_null(strstr(cwd_buf, "/simh-fio-home"));
    assert_int_equal(sim_stat(".", &cwd_statb), 0);
    assert_int_equal(cwd_statb.st_dev, statb.st_dev);
    assert_int_equal(cwd_statb.st_ino, statb.st_ino);

    assert_int_equal(chdir(fixture->original_cwd), 0);
    assert_int_equal(sim_rmdir(relative_home_dir), 0);
    assert_int_not_equal(sim_stat(relative_home_dir, &statb), 0);

    if (saved_home != NULL) {
        assert_int_equal(setenv("HOME", saved_home, 1), 0);
    } else {
        assert_int_equal(unsetenv("HOME"), 0);
    }
}

/* Verify the ASCII-safe character wrappers clamp non-ASCII input and
   preserve the expected behavior for simple ASCII characters. */
static void test_sim_character_class_helpers_are_ascii_safe(void **state)
{
    (void)state;

    assert_true(sim_isspace(' '));
    assert_false(sim_isspace(-1));
    assert_true(sim_islower('q'));
    assert_false(sim_islower('Q'));
    assert_true(sim_isupper('Q'));
    assert_false(sim_isupper('q'));
    assert_int_equal(sim_toupper('q'), 'Q');
    assert_int_equal(sim_tolower('Q'), 'q');
    assert_true(sim_isalpha('Z'));
    assert_false(sim_isalpha(0xC0));
    assert_true(sim_isprint('!'));
    assert_false(sim_isprint('\n'));
    assert_true(sim_isdigit('7'));
    assert_false(sim_isdigit('x'));
    assert_true(sim_isgraph('!'));
    assert_false(sim_isgraph(' '));
    assert_true(sim_isalnum('A'));
    assert_true(sim_isalnum('4'));
    assert_false(sim_isalnum(0x80));
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
        cmocka_unit_test_setup_teardown(
            test_sim_filepath_parts_handles_home_expansion_and_time_fields,
            setup_sim_fio_fixture, teardown_sim_fio_fixture),
        cmocka_unit_test(test_sim_byte_swap_helpers_reverse_multibyte_elements),
        cmocka_unit_test_setup_teardown(
            test_sim_fread_and_fwrite_round_trip_in_big_endian_mode,
            setup_sim_fio_fixture, teardown_sim_fio_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_fwrite_handles_multiple_flip_buffers,
            setup_sim_fio_fixture, teardown_sim_fio_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_file_size_helpers_report_sizes_consistently,
            setup_sim_fio_fixture, teardown_sim_fio_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_fopen_and_sim_set_fsize_handle_home_expansion,
            setup_sim_fio_fixture, teardown_sim_fio_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_file_times_updates_stat_times, setup_sim_fio_fixture,
            teardown_sim_fio_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_fifo_nonblock_marks_fifo_streams,
            setup_sim_fio_fixture, teardown_sim_fio_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_dir_scan_get_filelist_and_copyfile_work_together,
            setup_sim_fio_fixture, teardown_sim_fio_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_dir_scan_reports_missing_matches, setup_sim_fio_fixture,
            teardown_sim_fio_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_directory_wrappers_handle_home_expansion,
            setup_sim_fio_fixture, teardown_sim_fio_fixture),
        cmocka_unit_test(test_sim_character_class_helpers_are_ascii_safe),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
