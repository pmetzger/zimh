#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cmocka.h>

#include "scp.h"
#include "test_simh_personality.h"
#include "test_support.h"

static const char simh_test_prog_name[] = "/test/bin/simh-unit-scp-subst";

/* Keep substitution tests isolated from inherited process state. */
static int setup_scp_subst_fixture(void **state)
{
    (void)state;

    unsetenv("ZIMH_TEST_EXTERNAL");
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
    return 0;
}

/* Remove test variables and reset SCP globals after each test. */
static int teardown_scp_subst_fixture(void **state)
{
    (void)state;

    unsetenv("ZIMH_TEST_EXTERNAL");
    unsetenv("SIM_SEND_DELAY_CONSOLE");
    unsetenv("_FILE_COMPARE_DIFF_OFFSET");
    unsetenv("SIM_OSTYPE");
    unsetenv("SIM_NAME");
    unsetenv("SIM_BIN_NAME");
    unsetenv("SIM_BIN_PATH");
    unsetenv("SIM_DELTA");
    unsetenv("SIM_VERSION_MODE");
    simh_test_reset_simulator_state();
    return 0;
}

/* Expand one SCP command string with an otherwise empty argument vector. */
static void expand_command(const char *input, char *output, size_t size)
{
    char *do_arg[10] = {0};

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

    assert_null(getenv("_FILE_COMPARE_DIFF_OFFSET"));
    expand_command("A%_FILE_COMPARE_DIFF_OFFSET%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "A2B");

    unlink(path1);
    unlink(path2);
}

/* Verify RUNLIMIT is still available through substitution, not environment. */
static void test_runlimit_cmd_expands_without_environment(void **state)
{
    char expanded[CBUFSIZE];

    (void)state;

    assert_int_equal(run_scp_command("RUNLIMIT", "5"), SCPE_OK);
    assert_null(getenv("SIM_RUNLIMIT"));
    assert_null(getenv("SIM_RUNLIMIT_UNITS"));

    expand_command("A%SIM_RUNLIMIT%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "A5B");

    expand_command("A%SIM_RUNLIMIT_UNITS%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AinstructionsB");

    assert_int_equal(run_scp_command("NORUNLIMIT", ""), SCPE_OK);
    expand_command("A%SIM_RUNLIMIT%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
}

/* Verify RUNLIMIT still avoids publishing its state through environment vars. */
static void test_runlimit_cmd_does_not_publish_environment(void **state)
{
    (void)state;

    assert_int_equal(run_scp_command("RUNLIMIT", "5"), SCPE_OK);
    assert_null(getenv("SIM_RUNLIMIT"));
    assert_null(getenv("SIM_RUNLIMIT_UNITS"));

    assert_int_equal(run_scp_command("NORUNLIMIT", ""), SCPE_OK);
    assert_null(getenv("SIM_RUNLIMIT"));
    assert_null(getenv("SIM_RUNLIMIT_UNITS"));
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
    assert_true(strlen(expanded) > 2);
    assert_true(expanded[0] == 'A');
    assert_true(expanded[strlen(expanded) - 1] == 'B');
    assert_string_equal(expanded, "Asimh-unitB");

    expand_command("A%SIM_BIN_NAME%B", expanded, sizeof(expanded));
    assert_true(strlen(expanded) > 2);
    assert_true(expanded[0] == 'A');
    assert_true(expanded[strlen(expanded) - 1] == 'B');
    assert_string_equal(expanded, "Asimh-unit-scp-substB");

    expand_command("A%SIM_BIN_PATH%B", expanded, sizeof(expanded));
    assert_true(strlen(expanded) > 2);
    assert_true(expanded[0] == 'A');
    assert_true(expanded[strlen(expanded) - 1] == 'B');
    assert_string_equal(expanded, "A/test/bin/simh-unit-scp-substB");

    expand_command("SIM_NAME tail", expanded, sizeof(expanded));
    assert_string_equal(expanded, "simh-unit tail");
}

/* Verify SIM_OSTYPE still expands while removed SIM_* metadata stays hidden. */
static void test_show_version_keeps_only_sim_ostype(void **state)
{
    char expanded[CBUFSIZE];
    FILE *stream;

    (void)state;

    stream = tmpfile();
    assert_non_null(stream);

    assert_int_equal(show_version(stream, NULL, NULL, 1, NULL), SCPE_OK);
    fclose(stream);
    assert_null(getenv("SIM_OSTYPE"));
    assert_null(getenv("SIM_DELTA"));
    assert_null(getenv("SIM_VERSION_MODE"));
    assert_null(getenv("SIM_MAJOR"));
    assert_null(getenv("SIM_MINOR"));
    assert_null(getenv("SIM_PATCH"));
    assert_null(getenv("SIM_VM_RELEASE"));
    assert_null(getenv("SIM_GIT_COMMIT_ID"));
    assert_null(getenv("SIM_GIT_COMMIT_TIME"));
    assert_null(getenv("SIM_ARCHIVE_GIT_COMMIT_ID"));
    assert_null(getenv("SIM_ARCHIVE_GIT_COMMIT_TIME"));

    expand_command("A%SIM_OSTYPE%B", expanded, sizeof(expanded));
    assert_true(strlen(expanded) > 2);
    assert_true(expanded[0] == 'A');
    assert_true(expanded[strlen(expanded) - 1] == 'B');

    expand_command("A%DATE_YC%B", expanded, sizeof(expanded));
    assert_string_equal(expanded, "AB");
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_expands_external_environment,
            setup_scp_subst_fixture, teardown_scp_subst_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_omits_missing_send_default,
            setup_scp_subst_fixture, teardown_scp_subst_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_omits_missing_file_compare_offset,
            setup_scp_subst_fixture, teardown_scp_subst_fixture),
        cmocka_unit_test_setup_teardown(
            test_file_compare_sets_diff_offset_without_environment,
            setup_scp_subst_fixture, teardown_scp_subst_fixture),
        cmocka_unit_test_setup_teardown(
            test_runlimit_cmd_expands_without_environment,
            setup_scp_subst_fixture, teardown_scp_subst_fixture),
        cmocka_unit_test_setup_teardown(
            test_runlimit_cmd_does_not_publish_environment,
            setup_scp_subst_fixture, teardown_scp_subst_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_expands_utime,
            setup_scp_subst_fixture, teardown_scp_subst_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_sub_args_expands_sim_identity_variables,
            setup_scp_subst_fixture, teardown_scp_subst_fixture),
        cmocka_unit_test_setup_teardown(
            test_show_version_keeps_only_sim_ostype,
            setup_scp_subst_fixture, teardown_scp_subst_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
