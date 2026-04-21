#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cmocka.h>

#include "scp.h"
#include "sim_console.h"
#include "test_simh_personality.h"
#include "test_support.h"

static const char simh_test_prog_name[] = "/test/bin/simh-unit-scp-cmdvars";
static int simh_test_uname_probe_calls = 0;

static t_bool simh_test_ostype_probe_fail(char *buf, size_t size)
{
    (void)buf;
    (void)size;

    ++simh_test_uname_probe_calls;
    return FALSE;
}

static t_bool simh_test_ostype_probe_cached(char *buf, size_t size)
{
    ++simh_test_uname_probe_calls;
    strlcpy(buf, "ProbeOS", size);
    return TRUE;
}

/* Keep command-variable tests isolated from inherited process state. */
static int setup_scp_cmdvars_fixture(void **state)
{
    (void)state;

    unsetenv("ZIMH_TEST_EXTERNAL");
    unsetenv("ZIMH_TEST_SET");
    unsetenv("ZIMH_ALIAS");
    unsetenv("SIM_SEND_DELAY_CONSOLE");
    unsetenv("_FILE_COMPARE_DIFF_OFFSET");
    unsetenv("SIM_OSTYPE");
    unsetenv("SIM_NAME");
    unsetenv("SIM_BIN_NAME");
    unsetenv("SIM_BIN_PATH");
    unsetenv("SIM_DELTA");
    unsetenv("SIM_VERSION_MODE");
    simh_test_reset_simulator_state();
    sim_prog_name = simh_test_prog_name;
    sim_switches = 0;
    sim_rem_cmd_active_line = -1;
    simh_test_uname_probe_calls = 0;
    return 0;
}

/* Remove test variables and reset SCP globals after each test. */
static int teardown_scp_cmdvars_fixture(void **state)
{
    (void)state;

    unsetenv("ZIMH_TEST_EXTERNAL");
    unsetenv("ZIMH_TEST_SET");
    unsetenv("ZIMH_ALIAS");
    unsetenv("SIM_SEND_DELAY_CONSOLE");
    unsetenv("_FILE_COMPARE_DIFF_OFFSET");
    unsetenv("SIM_OSTYPE");
    unsetenv("SIM_NAME");
    unsetenv("SIM_BIN_NAME");
    unsetenv("SIM_BIN_PATH");
    unsetenv("SIM_DELTA");
    unsetenv("SIM_VERSION_MODE");
    simh_test_reset_simulator_state();
    sim_switches = 0;
    sim_rem_cmd_active_line = -1;
    simh_test_uname_probe_calls = 0;
    return 0;
}

/* Expand one SCP command string with an otherwise empty argument vector. */
static void expand_command(const char *input, char *output, size_t size)
{
    char *do_arg[10] = {0};

    strlcpy(output, input, size);
    sim_sub_args(output, size, do_arg);
}

/* Expand one SCP command string with a caller-supplied DO argument vector. */
static void expand_command_with_args(const char *input, char *output,
                                     size_t size, char *do_arg[])
{
    strlcpy(output, input, size);
    sim_sub_args(output, size, do_arg);
}

/* Run one top-level SCP command through its registered command action. */
static t_stat run_scp_command(const char *name, const char *args)
{
    CTAB *cmd = find_cmd(name);

    assert_non_null(cmd);
    return cmd->action(cmd->arg, args);
}

/* Create a small temporary file containing the provided bytes. */
static void write_temp_file(char *path, size_t size, const char *contents)
{
    int fd = mkstemp(path);
    FILE *stream;

    assert_true(fd >= 0);
    stream = fdopen(fd, "wb");
    assert_non_null(stream);
    assert_int_equal(fwrite(contents, 1, size, stream), size);
    fclose(stream);
}

/* Read one small text file into a fixed local buffer. */
static void read_text_file(const char *path, char *buffer, size_t size)
{
    void *data;
    size_t bytes;

    assert_int_equal(simh_test_read_file(path, &data, &bytes), 0);
    assert_true(bytes < size);
    memcpy(buffer, data, bytes + 1);
    free(data);
}

/* Temporarily replace stdin with a small prepared input file. */
static void with_redirected_stdin(const char *contents,
                                  void (*fn)(void *ctx), void *ctx)
{
    char path[] = "/tmp/zimh-cmdvars-stdin-XXXXXX";
    int saved_stdin;
    int fd;

    fd = mkstemp(path);
    assert_true(fd >= 0);
    assert_int_equal(write(fd, contents, strlen(contents)), (int)strlen(contents));
    assert_int_equal(lseek(fd, 0, SEEK_SET), 0);

    saved_stdin = dup(STDIN_FILENO);
    assert_true(saved_stdin >= 0);
    assert_int_equal(dup2(fd, STDIN_FILENO), STDIN_FILENO);
    close(fd);

    fn(ctx);

    assert_int_equal(dup2(saved_stdin, STDIN_FILENO), STDIN_FILENO);
    close(saved_stdin);
    unlink(path);
}

/* Verify ordinary host environment variables still expand normally. */
static void test_sim_sub_args_expands_external_environment(void **state)
{
    char expanded[CBUFSIZE];

    (void)state;

    assert_int_equal(setenv("ZIMH_TEST_EXTERNAL", "value", 1), 0);

    expand_command("A%ZIMH_TEST_EXTERNAL%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AvalueB");

    expand_command("ZIMH_TEST_EXTERNAL tail", expanded, sizeof(expanded));
    assert_string_equal(expanded, "value tail");
}

/* Verify lowercase names still fall back to uppercase host environment. */
static void test_sim_sub_args_expands_uppercased_external_environment(
    void **state)
{
    char expanded[CBUFSIZE];

    (void)state;

    assert_int_equal(setenv("ZIMH_TEST_UPPER", "upper", 1), 0);

    expand_command("A%zimh_test_upper%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AupperB");
}

/* Verify SCP-owned substitution variables can be set and removed directly. */
static void test_sim_sub_vars_set_get_and_unset(void **state)
{
    char value[CBUFSIZE];

    (void)state;

    sim_sub_var_set("ZIMH_LOCAL", "hello");
    assert_string_equal(
        _sim_get_env_special("ZIMH_LOCAL", value, sizeof(value)), "hello");

    sim_sub_var_set("ZIMH_LOCAL", "updated");
    assert_string_equal(
        _sim_get_env_special("ZIMH_LOCAL", value, sizeof(value)), "updated");

    sim_sub_var_unset("ZIMH_LOCAL");
    assert_null(_sim_get_env_special("ZIMH_LOCAL", value, sizeof(value)));
}

/* Verify prefix-based clearing removes only the matching variables. */
static void test_sim_sub_var_clear_prefix(void **state)
{
    char value[CBUFSIZE];

    (void)state;

    sim_sub_var_set("_EXPECT_MATCH_GROUP_0", "first");
    sim_sub_var_set("_EXPECT_MATCH_GROUP_1", "second");
    sim_sub_var_set("ZIMH_LOCAL", "keep");

    sim_sub_var_clear_prefix("_EXPECT_MATCH_");

    assert_null(
        _sim_get_env_special("_EXPECT_MATCH_GROUP_0", value, sizeof(value)));
    assert_null(
        _sim_get_env_special("_EXPECT_MATCH_GROUP_1", value, sizeof(value)));
    assert_string_equal(
        _sim_get_env_special("ZIMH_LOCAL", value, sizeof(value)), "keep");
}

/* Verify the documented date, time, and status built-ins still expand. */
static void test_sim_get_env_special_expands_documented_builtins(void **state)
{
    char seeded[CBUFSIZE];
    char value[CBUFSIZE];
    const char *always_nonempty[] = {
        "DATE",           "TIME",        "DATETIME",
        "LDATE",          "LTIME",       "CTIME",
        "UTIME",          "DATE_YYYY",   "DATE_YY",
        "DATE_19XX_YY",   "DATE_19XX_YYYY",
        "DATE_MM",        "DATE_MMM",    "DATE_MONTH",
        "DATE_DD",        "DATE_D",      "DATE_WW",
        "DATE_WYYYY",     "DATE_JJJ",    "TIME_HH",
        "TIME_MM",        "TIME_SS",     "TIME_MSEC",
        "STATUS",         "TSTATUS",
    };
    size_t i;

    (void)state;

    expand_command("seed", seeded, sizeof(seeded));
    sim_last_cmd_stat = SCPE_ARG;

    for (i = 0; i < sizeof(always_nonempty) / sizeof(always_nonempty[0]); ++i) {
        assert_non_null(_sim_get_env_special(always_nonempty[i], value,
                                             sizeof(value)));
        assert_true(value[0] != '\0');
    }
}

/* Verify mode-reporting variables reflect current SCP state. */
static void test_sim_get_env_special_expands_mode_variables(void **state)
{
    char seeded[CBUFSIZE];
    char value[CBUFSIZE];

    (void)state;

    expand_command("seed", seeded, sizeof(seeded));

    sim_quiet = 1;
    sim_show_message = 0;

    assert_non_null(_sim_get_env_special("SIM_VERIFY", value, sizeof(value)));
    assert_non_null(_sim_get_env_special("SIM_VERBOSE", value, sizeof(value)));

    assert_non_null(_sim_get_env_special("SIM_QUIET", value, sizeof(value)));
    assert_string_equal(value, "-Q");

    assert_non_null(_sim_get_env_special("SIM_MESSAGE", value, sizeof(value)));
    assert_string_equal(value, "-Q");
}

/* Verify SIM_BIN_NAME handles plain and missing program-name values. */
static void test_sim_get_env_special_handles_sim_bin_name_edge_cases(void **state)
{
    char value[CBUFSIZE];

    (void)state;

    sim_prog_name = "plain-name";
    assert_string_equal(
        _sim_get_env_special("SIM_BIN_NAME", value, sizeof(value)),
        "plain-name");

    sim_prog_name = NULL;
    assert_null(_sim_get_env_special("SIM_BIN_NAME", value, sizeof(value)));
}

/* Verify SIM_BIN_NAME strips Windows-style path prefixes too. */
static void test_sim_get_env_special_handles_windows_sim_bin_name(
    void **state)
{
    char value[CBUFSIZE];

    (void)state;

    sim_prog_name = "C:\\test\\bin\\simh-unit.exe";
    assert_string_equal(
        _sim_get_env_special("SIM_BIN_NAME", value, sizeof(value)),
        "simh-unit.exe");
}

/* Verify substring extraction and replacement modifiers still work. */
static void test_sim_sub_args_expands_substrings_and_replacements(void **state)
{
    char expanded[CBUFSIZE];

    (void)state;

    assert_int_equal(setenv("ZIMH_TEST_EXTERNAL", "abcdef", 1), 0);

    expand_command("A%ZIMH_TEST_EXTERNAL:~1,3%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AbcdB");

    expand_command("A%ZIMH_TEST_EXTERNAL:cd=XY%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AabXYefB");
}

/* Verify less common substring modifiers still behave as documented. */
static void
test_sim_sub_args_expands_negative_and_global_substring_modifiers(void **state)
{
    char expanded[CBUFSIZE];

    (void)state;

    assert_int_equal(setenv("ZIMH_TEST_EXTERNAL", "abcdef", 1), 0);

    expand_command("A%ZIMH_TEST_EXTERNAL:~-2,2%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AefB");

    expand_command("A%ZIMH_TEST_EXTERNAL:~1,-1%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AbcdefB");

    expand_command("A%ZIMH_TEST_EXTERNAL:~%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AabcdefB");

    expand_command("A%ZIMH_TEST_EXTERNAL:*cd=XY%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AXYefB");
}

/* Verify substring modifiers clamp overlong ranges cleanly. */
static void test_sim_sub_args_clamps_overlong_substrings(void **state)
{
    char expanded[CBUFSIZE];

    (void)state;

    assert_int_equal(setenv("ZIMH_TEST_EXTERNAL", "abcdef", 1), 0);

    expand_command("A%ZIMH_TEST_EXTERNAL:~2,99%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AcdefB");
}

/* Verify captured host aliases remain visible to SCP substitution. */
static void test_sim_get_env_special_expands_captured_aliases(void **state)
{
    char value[CBUFSIZE];

    (void)state;

    assert_int_equal(setenv("ZIMH_ALIAS", "from-host", 1), 0);
    sim_cmdvars_capture_env_alias("ZIMH_ALIAS");

    assert_string_equal(
        _sim_get_env_special("ZIMH_ALIAS", value, sizeof(value)), "from-host");
}

/* Verify missing aliases are ignored instead of creating empty entries. */
static void test_sim_cmdvars_capture_env_alias_ignores_missing_environment(
    void **state)
{
    char value[CBUFSIZE];

    (void)state;

    sim_cmdvars_capture_env_alias("ZIMH_ALIAS");
    assert_null(_sim_get_env_special("ZIMH_ALIAS", value, sizeof(value)));
}

/* Verify SCP does not publish SEND defaults unless it has a real value. */
static void test_sim_sub_args_omits_missing_send_default(void **state)
{
    char expanded[CBUFSIZE];

    (void)state;

    expand_command("A%SIM_SEND_DELAY_CONSOLE%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");

    expand_command("A%sim_send_delay_console%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");

    expand_command("SIM_SEND_DELAY_CONSOLE tail", expanded, sizeof(expanded));
    assert_string_equal(expanded, "SIM_SEND_DELAY_CONSOLE tail");
}

/* Verify FILE COMPARE scratch state is absent until a compare sets it. */
static void test_sim_sub_args_omits_missing_file_compare_offset(void **state)
{
    char expanded[CBUFSIZE];

    (void)state;

    expand_command("A%_FILE_COMPARE_DIFF_OFFSET%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");

    expand_command("_FILE_COMPARE_DIFF_OFFSET tail", expanded,
                   sizeof(expanded));
    assert_string_equal(expanded, "_FILE_COMPARE_DIFF_OFFSET tail");
}

/* Verify FILE COMPARE publishes the first differing offset via substitution. */
static void test_file_compare_sets_diff_offset_without_environment(void **state)
{
    char path1[] = "/tmp/zimh-filecmp-1-XXXXXX";
    char path2[] = "/tmp/zimh-filecmp-2-XXXXXX";
    char quoted1[CBUFSIZE];
    char quoted2[CBUFSIZE];
    char expanded[CBUFSIZE];
    int saved_switches;

    (void)state;

    write_temp_file(path1, 4, "abXd");
    write_temp_file(path2, 4, "abYd");

    snprintf(quoted1, sizeof(quoted1), "\"%s\"", path1);
    snprintf(quoted2, sizeof(quoted2), "\"%s\"", path2);

    saved_switches = sim_switches;
    sim_switches |= SWMASK('F');
    assert_int_not_equal(sim_cmp_string(quoted1, quoted2), 0);
    sim_switches = saved_switches;

    expand_command("A%_FILE_COMPARE_DIFF_OFFSET%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "A2B");

    unlink(path1);
    unlink(path2);
}

/* Verify RUNLIMIT is still available through substitution. */
static void test_runlimit_cmd_expands_without_environment(void **state)
{
    char expanded[CBUFSIZE];

    (void)state;

    assert_int_equal(run_scp_command("RUNLIMIT", "5"), SCPE_OK);

    expand_command("A%SIM_RUNLIMIT%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "A5B");

    expand_command("A%SIM_RUNLIMIT_UNITS%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AinstructionsB");

    assert_int_equal(run_scp_command("NORUNLIMIT", ""), SCPE_OK);
    expand_command("A%SIM_RUNLIMIT%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
}

/* Verify UTIME remains available for scripts that depend on it. */
static void test_sim_sub_args_expands_utime(void **state)
{
    char expanded[CBUFSIZE];
    size_t i;

    (void)state;

    expand_command("A%UTIME%B", expanded, sizeof(expanded));
    assert_true(strlen(expanded) > 2);
    assert_true(expanded[0] == 'A');
    assert_true(expanded[strlen(expanded) - 1] == 'B');

    for (i = 1; i + 1 < strlen(expanded); ++i)
        assert_true(expanded[i] >= '0' && expanded[i] <= '9');
}

/* Verify documented simulator identity variables come from SCP state. */
static void test_sim_sub_args_expands_sim_identity_variables(void **state)
{
    char expanded[CBUFSIZE];

    (void)state;

    assert_int_equal(setenv("SIM_NAME", "host-name", 1), 0);
    assert_int_equal(setenv("SIM_BIN_NAME", "host-bin", 1), 0);
    assert_int_equal(setenv("SIM_BIN_PATH", "/host/bin", 1), 0);

    expand_command("A%SIM_NAME%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "Asimh-unitB");

    expand_command("A%SIM_BIN_NAME%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "Asimh-unit-scp-cmdvarsB");

    expand_command("A%SIM_BIN_PATH%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "A/test/bin/simh-unit-scp-cmdvarsB");

    expand_command("SIM_NAME tail", expanded, sizeof(expanded));
    assert_string_equal(expanded, "simh-unit tail");
}

/* Verify SIM_OSTYPE still expands while removed metadata stays absent. */
static void test_show_version_keeps_only_sim_ostype(void **state)
{
    char expanded[CBUFSIZE];
    FILE *stream;

    (void)state;

    stream = tmpfile();
    assert_non_null(stream);

    assert_int_equal(show_version(stream, NULL, NULL, 1, NULL), SCPE_OK);
    fclose(stream);

    expand_command("A%SIM_OSTYPE%B", expanded, sizeof(expanded));
    assert_true(strlen(expanded) > 2);
    assert_true(expanded[0] == 'A');
    assert_true(expanded[strlen(expanded) - 1] == 'B');

    expand_command("A%DATE_YC%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
    expand_command("A%SIM_DELTA%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
    expand_command("A%SIM_VERSION_MODE%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
    expand_command("A%SIM_MAJOR%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
    expand_command("A%SIM_MINOR%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
    expand_command("A%SIM_PATCH%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
    expand_command("A%SIM_VM_RELEASE%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
    expand_command("A%SIM_GIT_COMMIT_ID%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
    expand_command("A%SIM_GIT_COMMIT_TIME%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
    expand_command("A%SIM_ARCHIVE_GIT_COMMIT_ID%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
    expand_command("A%SIM_ARCHIVE_GIT_COMMIT_TIME%B", expanded,
                   sizeof(expanded));
    assert_string_equal(expanded, "AB");
}

/* Verify SIM_OSTYPE cleanly reports no value when uname probing fails. */
static void test_sim_get_env_special_ostype_handles_total_probe_failure(
    void **state)
{
    char value[CBUFSIZE];

    (void)state;

    sim_cmdvars_set_ostype_probe(simh_test_ostype_probe_fail);
    assert_null(_sim_get_env_special("SIM_OSTYPE", value, sizeof(value)));
    assert_int_equal(simh_test_uname_probe_calls, 1);
}

/* Verify SIM_OSTYPE caches the first discovered value. */
static void test_sim_get_env_special_ostype_uses_cached_value(void **state)
{
    char value[CBUFSIZE];

    (void)state;

    sim_cmdvars_set_ostype_probe(simh_test_ostype_probe_cached);
    assert_string_equal(
        _sim_get_env_special("SIM_OSTYPE", value, sizeof(value)), "ProbeOS");
    assert_int_equal(simh_test_uname_probe_calls, 1);

    sim_cmdvars_set_ostype_probe(simh_test_ostype_probe_fail);
    assert_string_equal(
        _sim_get_env_special("SIM_OSTYPE", value, sizeof(value)), "ProbeOS");
    assert_int_equal(simh_test_uname_probe_calls, 1);
}

/* Verify SET ENVIRONMENT stores a literal value. */
static void test_sim_set_environment_literal_value(void **state)
{
    (void)state;

    sim_switches = 0;
    assert_int_equal(sim_set_environment(0, "ZIMH_TEST_SET=hello"), SCPE_OK);
    assert_string_equal(getenv("ZIMH_TEST_SET"), "hello");
}

/* Verify SET ENVIRONMENT /S decodes a quoted string value. */
static void test_sim_set_environment_decodes_quoted_string(void **state)
{
    (void)state;

    sim_switches = SWMASK('S');
    assert_int_equal(sim_set_environment(0, "ZIMH_TEST_SET=\"a\\tb\""),
                     SCPE_OK);
    assert_memory_equal(getenv("ZIMH_TEST_SET"), "a\tb", 3);
}

/* Verify SET ENVIRONMENT /A evaluates arithmetic expressions. */
static void test_sim_set_environment_evaluates_expression(void **state)
{
    (void)state;

    sim_switches = SWMASK('A');
    assert_int_equal(sim_set_environment(0, "ZIMH_TEST_SET=1+2"), SCPE_OK);
    assert_string_equal(getenv("ZIMH_TEST_SET"), "3");
}

/* Verify SET ENVIRONMENT /P uses the default value in non-interactive mode. */
static void test_sim_set_environment_prompt_uses_default(void **state)
{
    (void)state;

    sim_switches = SWMASK('P');
    sim_rem_cmd_active_line = 0;
    assert_int_equal(sim_set_environment(0, "\"Prompt\" ZIMH_TEST_SET=default"),
                     SCPE_OK);
    assert_string_equal(getenv("ZIMH_TEST_SET"), "default");
}

/* Verify captured host aliases are restored only while spawning a child. */
static void test_sim_cmdvars_system_restores_captured_alias(void **state)
{
    char path[] = "/tmp/zimh-cmdvars-system-XXXXXX";
    char command[CBUFSIZE];
    char contents[64];
    int fd;

    (void)state;

    fd = mkstemp(path);
    assert_true(fd >= 0);
    close(fd);
    unlink(path);

    assert_int_equal(setenv("ZIMH_ALIAS", "from-host", 1), 0);
    sim_cmdvars_capture_env_alias("ZIMH_ALIAS");
    assert_null(getenv("ZIMH_ALIAS"));

    snprintf(command, sizeof(command),
             "/bin/sh -c 'printf %%s \"$ZIMH_ALIAS\" > \"%s\"'", path);
    assert_int_equal(sim_cmdvars_system(command), 0);

    read_text_file(path, contents, sizeof(contents));
    assert_string_equal(contents, "from-host");
    assert_null(getenv("ZIMH_ALIAS"));

    unlink(path);
}

/* Verify sim_cmdvars_system also works when there are no captured aliases. */
static void test_sim_cmdvars_system_without_captured_aliases(void **state)
{
    (void)state;

    assert_int_equal(sim_cmdvars_system("/usr/bin/true"), 0);
}

/* Verify SET ENVIRONMENT replaces a captured alias with the new real value. */
static void test_sim_set_environment_replaces_captured_alias(void **state)
{
    char path[] = "/tmp/zimh-cmdvars-alias-XXXXXX";
    char command[CBUFSIZE];
    char contents[64];
    int fd;

    (void)state;

    fd = mkstemp(path);
    assert_true(fd >= 0);
    close(fd);
    unlink(path);

    assert_int_equal(setenv("ZIMH_ALIAS", "from-host", 1), 0);
    sim_cmdvars_capture_env_alias("ZIMH_ALIAS");
    assert_null(getenv("ZIMH_ALIAS"));

    sim_switches = 0;
    assert_int_equal(sim_set_environment(0, "ZIMH_ALIAS=from-set"), SCPE_OK);
    assert_string_equal(getenv("ZIMH_ALIAS"), "from-set");

    snprintf(command, sizeof(command),
             "/bin/sh -c 'printf %%s \"$ZIMH_ALIAS\" > \"%s\"'", path);
    assert_int_equal(sim_cmdvars_system(command), 0);

    read_text_file(path, contents, sizeof(contents));
    assert_string_equal(contents, "from-set");

    unlink(path);
}

struct prompt_test_context {
    const char *command;
    const char *expected_value;
};

/* Exercise one interactive /P read against redirected stdin. */
static void run_prompt_environment_read(void *ctx)
{
    struct prompt_test_context *prompt_ctx = ctx;

    sim_switches = SWMASK('P');
    sim_rem_cmd_active_line = -1;
    assert_int_equal(sim_set_environment(0, prompt_ctx->command), SCPE_OK);
    assert_string_equal(getenv("ZIMH_TEST_SET"), prompt_ctx->expected_value);
}

/* Verify /P reads from stdin in interactive mode. */
static void test_sim_set_environment_prompt_reads_stdin(void **state)
{
    struct prompt_test_context prompt_ctx = {
        "\"Prompt\" ZIMH_TEST_SET=default",
        "typed"
    };

    (void)state;

    with_redirected_stdin("typed\n", run_prompt_environment_read, &prompt_ctx);
}

/* Verify /P falls back to the default when interactive input is empty. */
static void test_sim_set_environment_prompt_uses_default_after_empty_input(
    void **state)
{
    struct prompt_test_context prompt_ctx = {
        "\"Prompt\" ZIMH_TEST_SET=default",
        "default"
    };

    (void)state;

    with_redirected_stdin("\n", run_prompt_environment_read, &prompt_ctx);
}

/* Verify % expansion still handles escapes, arguments, and filepath parts. */
static void test_sim_sub_args_handles_escape_and_do_arguments(void **state)
{
    char path[] = "/tmp/zimh-cmdvars-path-XXXXXX.txt";
    char expanded[CBUFSIZE];
    char expected[CBUFSIZE];
    char *do_arg[10] = {0};
    char *path_only;
    char *name_only;
    char *ext_only;
    char *name_ext;
    int fd;

    (void)state;

    fd = mkstemps(path, 4);
    assert_true(fd >= 0);
    close(fd);

    do_arg[0] = "script";
    do_arg[1] = path;

    expand_command("A%%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "A%B");

    expand_command("trail %", expanded, sizeof(expanded));
    assert_string_equal(expanded, "trail %");

    expand_command_with_args("%1 tail", expanded, sizeof(expanded), do_arg);
    snprintf(expected, sizeof(expected), "%s tail", path);
    assert_string_equal(expanded, expected);

    path_only = sim_filepath_parts(path, "p");
    name_only = sim_filepath_parts(path, "n");
    ext_only = sim_filepath_parts(path, "x");
    name_ext = sim_filepath_parts(path, "nx");

    expand_command_with_args("%~p1", expanded, sizeof(expanded), do_arg);
    assert_string_equal(expanded, path_only);

    expand_command_with_args("%~n1", expanded, sizeof(expanded), do_arg);
    assert_string_equal(expanded, name_only);

    expand_command_with_args("%~x1", expanded, sizeof(expanded), do_arg);
    assert_string_equal(expanded, ext_only);

    expand_command_with_args("%~nx1", expanded, sizeof(expanded), do_arg);
    assert_string_equal(expanded, name_ext);

    free(path_only);
    free(name_only);
    free(ext_only);
    free(name_ext);
    unlink(path);
}

/* Verify missing DO arguments and %* expansion use the remaining args. */
static void test_sim_sub_args_handles_missing_and_star_do_arguments(
    void **state)
{
    char expanded[CBUFSIZE];
    char *do_arg[10] = {0};

    (void)state;

    do_arg[0] = "script";
    do_arg[1] = "alpha";
    do_arg[2] = "beta";
    do_arg[4] = "late";

    expand_command_with_args("A%3B", expanded, sizeof(expanded), do_arg);
    assert_string_equal(expanded, "AB");

    expand_command_with_args("A%*B", expanded, sizeof(expanded), do_arg);
    assert_string_equal(expanded, "Aalpha betaB");
}

/* Verify %n disappears if an earlier DO argument is omitted. */
static void test_sim_sub_args_omits_sparse_do_arguments(void **state)
{
    char expanded[CBUFSIZE];
    char *do_arg[10] = {0};

    (void)state;

    do_arg[0] = "script";
    do_arg[2] = "late";

    expand_command_with_args("A%2B", expanded, sizeof(expanded), do_arg);
    assert_string_equal(expanded, "AB");
}

/* Verify %* quotes spaced arguments and switches quote style if needed. */
static void test_sim_sub_args_star_quotes_spaced_arguments(void **state)
{
    char expanded[CBUFSIZE];
    char *do_arg[10] = {0};

    (void)state;

    do_arg[0] = "script";
    do_arg[1] = "two words";
    do_arg[2] = "he\"llo world";

    expand_command_with_args("A%*B", expanded, sizeof(expanded), do_arg);
    assert_string_equal(expanded, "A\"two words\" 'he\"llo world'B");
}

/* Verify a quoted whole-line command is decoded before substitution. */
static void test_sim_sub_args_decodes_single_quoted_argument(void **state)
{
    char expanded[CBUFSIZE];
    char *do_arg[10] = {0};

    (void)state;

    expand_command_with_args("\"a\\tb\"", expanded, sizeof(expanded), do_arg);
    assert_memory_equal(expanded, "a\tb", 3);
}

/* Verify whole-line quoted decode trims decoded leading whitespace. */
static void test_sim_sub_args_decodes_single_quoted_argument_with_spaces(
    void **state)
{
    char expanded[CBUFSIZE];
    char *do_arg[10] = {0};

    (void)state;

    expand_command_with_args("\"  a\"", expanded, sizeof(expanded), do_arg);
    assert_string_equal(expanded, "a");
}

/* Verify quoted-only commands with trailing tokens bypass decode cleanly. */
static void test_sim_sub_args_preserves_quoted_command_with_trailing_text(
    void **state)
{
    char expanded[CBUFSIZE];
    char *do_arg[10] = {0};

    (void)state;

    expand_command_with_args("\"a\" tail", expanded, sizeof(expanded), do_arg);
    assert_string_equal(expanded, "\"a\" tail");
}

/* Verify quoted commands skip intervening spaces before trailing text. */
static void test_sim_sub_args_preserves_quoted_command_after_space_skip(
    void **state)
{
    char expanded[CBUFSIZE];
    char *do_arg[10] = {0};

    (void)state;

    expand_command_with_args("\"a\"   tail", expanded, sizeof(expanded),
                             do_arg);
    assert_string_equal(expanded, "\"a\"   tail");
}

/* Verify quoted commands with trailing spaces skip whitespace cleanly. */
static void test_sim_sub_args_preserves_quoted_command_with_trailing_spaces(
    void **state)
{
    char expanded[CBUFSIZE];
    char *do_arg[10] = {0};

    (void)state;

    expand_command_with_args("\"a\"   ", expanded, sizeof(expanded), do_arg);
    assert_string_equal(expanded, "a");
}

/* Verify leading whitespace and quoted-only commands are preserved cleanly. */
static void test_sim_sub_args_handles_leading_space_and_quoted_command(
    void **state)
{
    char expanded[CBUFSIZE];
    char *do_arg[10] = {0};

    (void)state;

    expand_command_with_args("  \"a\\tb\"   ", expanded, sizeof(expanded),
                             do_arg);
    assert_string_equal(expanded, "  a\tb");
}

/* Verify the expanded-string bookkeeping can map literal output back. */
static void test_sim_unsub_args_preserves_literal_positions(void **state)
{
    char expanded[CBUFSIZE];

    (void)state;

    expand_command("A literal tail", expanded, sizeof(expanded));
    assert_string_equal(expanded, "A literal tail");
    assert_string_equal(sim_unsub_args(strchr(expanded, ' ')), " literal tail");
    assert_string_equal(sim_unsub_args("outside"), "outside");
}

/* Verify SET ENVIRONMENT reports missing arguments and bad quoted strings. */
static void test_sim_set_environment_rejects_bad_inputs(void **state)
{
    (void)state;

    sim_switches = 0;
    assert_int_equal(sim_set_environment(0, ""), SCPE_2FARG);

    sim_switches = SWMASK('S');
    assert_int_equal(
        SCPE_BARE_STATUS(
            sim_set_environment(0, "ZIMH_TEST_SET=\"unterminated")),
        SCPE_ARG);

    sim_switches = SWMASK('A');
    assert_int_equal(
        SCPE_BARE_STATUS(sim_set_environment(0, "ZIMH_TEST_SET=1 + )")),
        SCPE_ARG);
}

/* Verify SET ENVIRONMENT /P covers the remaining non-interactive forms. */
static void test_sim_set_environment_prompt_reports_missing_fields(void **state)
{
    (void)state;

    sim_switches = SWMASK('P');
    sim_rem_cmd_active_line = 0;

    assert_int_equal(
        SCPE_BARE_STATUS(sim_set_environment(0, "   ")), SCPE_2FARG);
    assert_int_equal(
        SCPE_BARE_STATUS(sim_set_environment(0, "\"\" ZIMH_TEST_SET=default")),
        SCPE_ARG);
    assert_int_equal(
        SCPE_BARE_STATUS(sim_set_environment(0, "\"Prompt\" ")), SCPE_2FARG);

    assert_int_equal(sim_set_environment(0, "\"Prompt\" ZIMH_TEST_SET"),
                     SCPE_OK);
    assert_string_equal(getenv("ZIMH_TEST_SET"), "");
}

/* Verify replacing one alias leaves other captured aliases intact. */
static void test_sim_set_environment_keeps_other_captured_aliases(void **state)
{
    char value[CBUFSIZE];

    (void)state;

    assert_int_equal(setenv("ZIMH_ALIAS", "from-host", 1), 0);
    assert_int_equal(setenv("ZIMH_TEST_EXTERNAL", "keep-host", 1), 0);
    sim_cmdvars_capture_env_alias("ZIMH_ALIAS");
    sim_cmdvars_capture_env_alias("ZIMH_TEST_EXTERNAL");

    sim_switches = 0;
    assert_int_equal(sim_set_environment(0, "ZIMH_ALIAS=from-set"), SCPE_OK);

    assert_string_equal(getenv("ZIMH_ALIAS"), "from-set");
    assert_string_equal(
        _sim_get_env_special("ZIMH_TEST_EXTERNAL", value, sizeof(value)),
        "keep-host");
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_expands_external_environment,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_expands_uppercased_external_environment,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(test_sim_sub_vars_set_get_and_unset,
                                        setup_scp_cmdvars_fixture,
                                        teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_get_env_special_expands_documented_builtins,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_get_env_special_expands_mode_variables,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_get_env_special_handles_sim_bin_name_edge_cases,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_get_env_special_handles_windows_sim_bin_name,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(test_sim_sub_var_clear_prefix,
                                        setup_scp_cmdvars_fixture,
                                        teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_expands_substrings_and_replacements,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_expands_negative_and_global_substring_modifiers,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_clamps_overlong_substrings,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_get_env_special_expands_captured_aliases,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_cmdvars_capture_env_alias_ignores_missing_environment,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_omits_missing_send_default,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_omits_missing_file_compare_offset,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_file_compare_sets_diff_offset_without_environment,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_runlimit_cmd_expands_without_environment,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(test_sim_sub_args_expands_utime,
                                        setup_scp_cmdvars_fixture,
                                        teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_expands_sim_identity_variables,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(test_show_version_keeps_only_sim_ostype,
                                        setup_scp_cmdvars_fixture,
                                        teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_get_env_special_ostype_handles_total_probe_failure,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_get_env_special_ostype_uses_cached_value,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(test_sim_set_environment_literal_value,
                                        setup_scp_cmdvars_fixture,
                                        teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_environment_decodes_quoted_string,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_environment_evaluates_expression,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_environment_prompt_uses_default,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_cmdvars_system_restores_captured_alias,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_cmdvars_system_without_captured_aliases,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_environment_replaces_captured_alias,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_environment_prompt_reads_stdin,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_environment_prompt_uses_default_after_empty_input,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_handles_escape_and_do_arguments,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_handles_missing_and_star_do_arguments,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_omits_sparse_do_arguments,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_star_quotes_spaced_arguments,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_decodes_single_quoted_argument,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_decodes_single_quoted_argument_with_spaces,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_preserves_quoted_command_with_trailing_text,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_preserves_quoted_command_after_space_skip,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_preserves_quoted_command_with_trailing_spaces,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_handles_leading_space_and_quoted_command,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_unsub_args_preserves_literal_positions,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_environment_rejects_bad_inputs,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_environment_prompt_reports_missing_fields,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_environment_keeps_other_captured_aliases,
            setup_scp_cmdvars_fixture, teardown_scp_cmdvars_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
