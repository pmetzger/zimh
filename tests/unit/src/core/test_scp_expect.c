#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "scp.h"
#include "scp_expect.h"
#include "test_scp_fixture.h"
#include "test_simh_personality.h"
#include "test_support.h"

struct scp_expect_fixture {
    DEVICE device;
    UNIT unit;
    EXPECT exp;
    SEND snd;
};

static void clear_expect_match_env(void)
{
    size_t i;
    char name[32];

    unsetenv("_EXPECT_MATCH_PATTERN");
    for (i = 0; i < 8; ++i) {
        snprintf(name, sizeof(name), "_EXPECT_MATCH_GROUP_%zu", i);
        unsetenv(name);
    }
}

static int setup_scp_expect_fixture(void **state)
{
    struct scp_expect_fixture *fixture;
    DEVICE *devices[2];

    fixture = calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    simh_test_init_device_unit(&fixture->device, &fixture->unit, "TTY", "TTY0",
                               0, 0, 8, 1);

    devices[0] = &fixture->device;
    devices[1] = NULL;
    assert_int_equal(
        simh_test_install_devices("simh-unit-scp-expect", devices), 0);

    fixture->exp.dptr = &fixture->device;
    fixture->snd.dptr = &fixture->device;
    assert_int_equal(sim_brk_init(), SCPE_OK);
    sim_switches = 0;
    sim_switch_number = 0;
    clear_expect_match_env();

    *state = fixture;
    return 0;
}

static int teardown_scp_expect_fixture(void **state)
{
    struct scp_expect_fixture *fixture = *state;

    sim_exp_clrall(&fixture->exp);
    sim_send_clear(&fixture->snd);
    free(fixture->snd.buffer);
    fixture->snd.buffer = NULL;
    sim_brk_clract();
    clear_expect_match_env();
    simh_test_reset_simulator_state();
    free(fixture);
    *state = NULL;
    return 0;
}

/* Verify SEND queues bytes and polls them back in order immediately. */
static void test_sim_send_input_queues_and_polls_bytes(void **state)
{
    struct scp_expect_fixture *fixture = *state;
    static uint8 payload[] = {'A', 'B', 'C'};
    t_stat stat;

    assert_int_equal(
        sim_send_input(&fixture->snd, payload, sizeof(payload), 0, 0), SCPE_OK);
    assert_int_equal(fixture->snd.insoff, sizeof(payload));

    assert_true(sim_send_poll_data(&fixture->snd, &stat));
    assert_int_equal(stat, 'A' | SCPE_KFLAG);
    assert_true(sim_send_poll_data(&fixture->snd, &stat));
    assert_int_equal(stat, 'B' | SCPE_KFLAG);
    assert_true(sim_send_poll_data(&fixture->snd, &stat));
    assert_int_equal(stat, 'C' | SCPE_KFLAG);
    assert_false(sim_send_poll_data(&fixture->snd, &stat));
}

/* Verify clearing SEND state discards any queued input immediately. */
static void test_sim_send_clear_discards_pending_bytes(void **state)
{
    struct scp_expect_fixture *fixture = *state;
    static uint8 payload[] = {'X', 'Y'};
    t_stat stat;

    assert_int_equal(
        sim_send_input(&fixture->snd, payload, sizeof(payload), 0, 0), SCPE_OK);
    assert_int_equal(sim_send_clear(&fixture->snd), SCPE_OK);
    assert_int_equal(fixture->snd.insoff, 0);
    assert_int_equal(fixture->snd.extoff, 0);
    assert_false(sim_send_poll_data(&fixture->snd, &stat));
}

/* Verify an exact-match rule triggers its action and then removes itself. */
static void test_sim_exp_check_triggers_action_and_consumes_rule(void **state)
{
    struct scp_expect_fixture *fixture = *state;
    char action[CBUFSIZE];

    assert_int_equal(
        sim_exp_set(&fixture->exp, "\"OK\"", 0, 0, 0, "echo matched"), SCPE_OK);
    assert_int_equal(fixture->exp.size, 1);

    assert_int_equal(sim_exp_check(&fixture->exp, 'O'), SCPE_OK);
    assert_int_equal(fixture->exp.size, 1);
    assert_null(sim_brk_getact(action, sizeof(action)));

    assert_int_equal(sim_exp_check(&fixture->exp, 'K'), SCPE_OK);
    assert_int_equal(fixture->exp.size, 0);
    assert_string_equal(sim_brk_getact(action, sizeof(action)), "echo matched");
}

/* Verify persistent duplicate rules are rejected to avoid ambiguity. */
static void test_sim_exp_set_rejects_duplicate_persistent_rules(void **state)
{
    struct scp_expect_fixture *fixture = *state;

    assert_int_equal(
        sim_exp_set(&fixture->exp, "\"READY\"", 0, 0, EXP_TYP_PERSIST, NULL),
        SCPE_OK);
    assert_int_equal(SCPE_BARE_STATUS(sim_exp_set(&fixture->exp, "\"READY\"", 0,
                                                  0, EXP_TYP_PERSIST, NULL)),
                     SCPE_ARG);
}

/* Verify explicit clear and clear-all both leave a clean expect context. */
static void test_sim_exp_clear_paths_remove_rules_and_buffers(void **state)
{
    struct scp_expect_fixture *fixture = *state;

    assert_int_equal(
        sim_exp_set(&fixture->exp, "\"A\"", 0, 0, EXP_TYP_PERSIST, NULL),
        SCPE_OK);
    assert_int_equal(
        sim_exp_set(&fixture->exp, "\"B\"", 0, 0, EXP_TYP_PERSIST, NULL),
        SCPE_OK);
    assert_int_equal(fixture->exp.size, 2);

    assert_int_equal(sim_exp_clr(&fixture->exp, "\"A\""), SCPE_OK);
    assert_int_equal(fixture->exp.size, 1);

    assert_int_equal(sim_exp_check(&fixture->exp, 'X'), SCPE_OK);
    assert_true(fixture->exp.buf_ins > 0);

    assert_int_equal(sim_exp_clrall(&fixture->exp), SCPE_OK);
    assert_int_equal(fixture->exp.size, 0);
    assert_null(fixture->exp.rules);
    assert_null(fixture->exp.buf);
    assert_int_equal(fixture->exp.buf_ins, 0);
}

#if defined(USE_REGEX)
/* Verify regex expect rules capture groups into the documented environment. */
static void test_sim_exp_check_populates_regex_capture_groups(void **state)
{
    struct scp_expect_fixture *fixture = *state;

    assert_int_equal(sim_exp_set(&fixture->exp, "/ab([0-9][0-9])/", 0, 0,
                                 EXP_TYP_REGEX, NULL),
                     SCPE_OK);

    assert_int_equal(sim_exp_check(&fixture->exp, 'a'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'b'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, '4'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, '2'), SCPE_OK);

    assert_string_equal(getenv("_EXPECT_MATCH_PATTERN"), "/ab([0-9][0-9])/");
    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_0"), "ab42");
    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_1"), "42");
}

/* Verify regex rules honor case-independent matching when requested. */
static void test_sim_exp_check_honors_case_independent_regex(void **state)
{
    struct scp_expect_fixture *fixture = *state;

    assert_int_equal(sim_exp_set(&fixture->exp, "/ab([0-9])/", 0, 0,
                                 EXP_TYP_REGEX | EXP_TYP_REGEX_I, NULL),
                     SCPE_OK);

    assert_int_equal(sim_exp_check(&fixture->exp, 'A'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'B'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, '5'), SCPE_OK);

    assert_string_equal(getenv("_EXPECT_MATCH_PATTERN"), "/ab([0-9])/");
    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_0"), "AB5");
    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_1"), "5");
}

/* Verify invalid regex syntax is rejected without installing a rule. */
static void test_sim_exp_set_rejects_invalid_regex_syntax(void **state)
{
    struct scp_expect_fixture *fixture = *state;

    assert_int_equal(SCPE_BARE_STATUS(sim_exp_set(&fixture->exp, "/(/", 0, 0,
                                                  EXP_TYP_REGEX, NULL)),
                     SCPE_ARG);
    assert_int_equal(fixture->exp.size, 0);
    assert_null(fixture->exp.rules);
}

/* Verify later regex matches clear stale capture groups from prior rules. */
static void test_sim_exp_check_clears_stale_regex_capture_groups(void **state)
{
    struct scp_expect_fixture *fixture = *state;

    assert_int_equal(sim_exp_set(&fixture->exp, "/ab([0-9])([0-9])/", 0, 0,
                                 EXP_TYP_REGEX, NULL),
                     SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'a'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'b'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, '4'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, '2'), SCPE_OK);

    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_0"), "ab42");
    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_1"), "4");
    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_2"), "2");

    assert_int_equal(sim_exp_set(&fixture->exp, "/cd([0-9])/", 0, 0,
                                 EXP_TYP_REGEX, NULL),
                     SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'c'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'd'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, '7'), SCPE_OK);

    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_0"), "cd7");
    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_1"), "7");
    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_2"), "");
}
#endif

/* Verify the SHOW helpers describe pending SEND and EXPECT state. */
static void test_show_helpers_render_pending_state(void **state)
{
    struct scp_expect_fixture *fixture = *state;
    static uint8 payload[] = {'Z'};
    FILE *stream;
    char *text;
    size_t size;

    assert_int_equal(
        sim_exp_set(&fixture->exp, "\"Z\"", 0, 0, EXP_TYP_PERSIST, NULL),
        SCPE_OK);
    assert_int_equal(
        sim_send_input(&fixture->snd, payload, sizeof(payload), 0, 0), SCPE_OK);

    stream = tmpfile();
    assert_non_null(stream);
    assert_int_equal(sim_show_send_input(stream, &fixture->snd), SCPE_OK);
    assert_int_equal(simh_test_read_stream(stream, &text, &size), 0);
    assert_non_null(strstr(text, "pending input Data"));
    free(text);
    fclose(stream);

    stream = tmpfile();
    assert_non_null(stream);
    assert_int_equal(sim_exp_show(stream, &fixture->exp, ""), SCPE_OK);
    assert_int_equal(simh_test_read_stream(stream, &text, &size), 0);
    assert_non_null(strstr(text, "\"Z\""));
    free(text);
    fclose(stream);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_sim_send_input_queues_and_polls_bytes,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_send_clear_discards_pending_bytes,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_exp_check_triggers_action_and_consumes_rule,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_exp_set_rejects_duplicate_persistent_rules,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_exp_clear_paths_remove_rules_and_buffers,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
#if defined(USE_REGEX)
        cmocka_unit_test_setup_teardown(
            test_sim_exp_check_populates_regex_capture_groups,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_exp_check_honors_case_independent_regex,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_exp_set_rejects_invalid_regex_syntax,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_exp_check_clears_stale_regex_capture_groups,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
#endif
        cmocka_unit_test_setup_teardown(test_show_helpers_render_pending_state,
                                        setup_scp_expect_fixture,
                                        teardown_scp_expect_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
