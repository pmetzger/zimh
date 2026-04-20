#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "scp.h"
#include "scp_expect.h"
#include "sim_console.h"
#include "test_scp_fixture.h"
#include "test_simh_personality.h"
#include "test_support.h"

struct scp_expect_fixture {
    DEVICE device;
    UNIT unit;
    EXPECT exp;
    SEND snd;
};

static void clear_send_expect_default_env(void)
{
    unsetenv("SIM_SEND_DELAY_CONSOLE");
    unsetenv("SIM_SEND_AFTER_CONSOLE");
    unsetenv("SIM_EXPECT_HALTAFTER_CONSOLE");
}

static void reset_send_context(SEND *snd)
{
    sim_send_clear(snd);
    free(snd->buffer);
    snd->buffer = NULL;
    snd->bufsize = 0;
    snd->delay = 0;
    snd->after = 0;
    snd->next_time = 0.0;
}

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
    clear_send_expect_default_env();
    assert_int_equal(sim_exp_clrall(sim_cons_get_expect()), SCPE_OK);
    reset_send_context(sim_cons_get_send());

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
    assert_int_equal(sim_exp_clrall(sim_cons_get_expect()), SCPE_OK);
    reset_send_context(sim_cons_get_send());
    sim_brk_clract();
    clear_expect_match_env();
    clear_send_expect_default_env();
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

/* Verify SEND command parsing updates console defaults and queue timing. */
static void test_send_cmd_sets_console_defaults_and_queue_timing(void **state)
{
    SEND *snd;

    (void)state;

    snd = sim_cons_get_send();
    assert_int_equal(send_cmd(1, "DELAY=7 AFTER=11"), SCPE_OK);
    assert_string_equal(getenv("SIM_SEND_DELAY_CONSOLE"), "7");
    assert_string_equal(getenv("SIM_SEND_AFTER_CONSOLE"), "11");

    assert_int_equal(send_cmd(1, "\"AZ\""), SCPE_OK);
    assert_int_equal(snd->delay, 7);
    assert_int_equal(snd->after, 11);
    assert_int_equal(snd->insoff, 2);
}

/* Verify SEND command parsing rejects malformed arguments and extras. */
static void test_send_cmd_rejects_invalid_argument_forms(void **state)
{
    (void)state;

    assert_int_equal(send_cmd(1, ""), SCPE_2FARG);
    assert_int_equal(SCPE_BARE_STATUS(send_cmd(1, "plain")), SCPE_ARG);
    assert_int_equal(SCPE_BARE_STATUS(send_cmd(1, "AFTER=1 plain")),
                     SCPE_ARG);
    assert_int_equal(send_cmd(1, "\"A\" extra"), SCPE_2MARG);
    assert_int_equal(SCPE_BARE_STATUS(send_cmd(1, "\"\\q\"")), SCPE_ARG);
    assert_int_equal(
        SCPE_BARE_STATUS(send_cmd(1, "DELAY=bogus \"A\"")), SCPE_ARG);
    assert_int_equal(
        SCPE_BARE_STATUS(send_cmd(1, "AFTER=bogus \"A\"")), SCPE_ARG);
}

/* Verify the SEND -t switch converts microseconds into instruction time. */
static void test_send_cmd_time_switch_converts_usec_to_instructions(void **state)
{
    SEND *snd;
    uint32 expected_delay;
    uint32 expected_after;

    (void)state;

    snd = sim_cons_get_send();
    expected_delay =
        (uint32)((sim_timer_inst_per_sec() * 2000) / 1000000.0);
    expected_after =
        (uint32)((sim_timer_inst_per_sec() * 3000) / 1000000.0);

    assert_int_equal(send_cmd(1, "-t DELAY=2000 AFTER=3000 \"A\""), SCPE_OK);
    assert_int_equal(snd->delay, expected_delay);
    assert_int_equal(snd->after, expected_after);
}

/* Verify EXPECT parses repeat counts, HALTAFTER, and trailing actions. */
static void test_sim_set_expect_parses_repeat_haltafter_and_action(void **state)
{
    EXPECT *exp;
    char action[CBUFSIZE];

    (void)state;

    exp = sim_cons_get_expect();
    assert_int_equal(sim_set_expect(exp, "HALTAFTER=13"), SCPE_OK);
    assert_string_equal(getenv("SIM_EXPECT_HALTAFTER_CONSOLE"), "13");

    assert_int_equal(sim_set_expect(exp, "[2] HALTAFTER=9 \"GO\" echo hit"),
                     SCPE_OK);
    assert_int_equal(exp->size, 1);
    assert_int_equal(exp->rules[0].cnt, 1);
    assert_int_equal(exp->rules[0].after, 9);
    assert_string_equal(exp->rules[0].act, "echo hit");

    sim_brk_clract();
    assert_int_equal(sim_exp_check(exp, 'G'), SCPE_OK);
    assert_int_equal(sim_exp_check(exp, 'O'), SCPE_OK);
    assert_int_equal(exp->rules[0].cnt, 0);
    assert_null(sim_brk_getact(action, sizeof(action)));

    assert_int_equal(sim_exp_check(exp, 'G'), SCPE_OK);
    assert_int_equal(sim_exp_check(exp, 'O'), SCPE_OK);
    assert_string_equal(sim_brk_getact(action, sizeof(action)), "echo hit");
}

/* Verify EXPECT parser errors and -t switch handling on the console rule. */
static void test_expect_cmd_parses_switches_and_rejects_bad_input(void **state)
{
    EXPECT *exp;

    (void)state;

    exp = sim_cons_get_expect();
    assert_int_equal(
        SCPE_BARE_STATUS(expect_cmd(1, "[0] \"A\" action")), SCPE_ARG);
    assert_int_equal(
        SCPE_BARE_STATUS(expect_cmd(1, "HALTAFTER=bogus \"A\"")), SCPE_ARG);
    assert_int_equal(SCPE_BARE_STATUS(expect_cmd(1, "plain")), SCPE_ARG);

    assert_int_equal(expect_cmd(1, "-t HALTAFTER=1000 \"A\""), SCPE_OK);
    assert_int_equal(exp->size, 1);
    assert_true(exp->rules[0].switches & EXP_TYP_TIME);
    assert_int_equal(exp->rules[0].after, 1000);
}

/* Verify NOEXPECT clears one rule or all rules through the parser path. */
static void test_sim_set_noexpect_removes_specific_rules_and_all(void **state)
{
    EXPECT *exp;

    (void)state;

    exp = sim_cons_get_expect();
    assert_int_equal(sim_exp_set(exp, "\"A\"", 0, 0, EXP_TYP_PERSIST, NULL),
                     SCPE_OK);
    assert_int_equal(sim_exp_set(exp, "\"B\"", 0, 0, EXP_TYP_PERSIST, NULL),
                     SCPE_OK);
    assert_int_equal(exp->size, 2);

    assert_int_equal(sim_set_noexpect(exp, "\"A\""), SCPE_OK);
    assert_int_equal(exp->size, 1);
    assert_string_equal(exp->rules[0].match_pattern, "\"B\"");

    assert_int_equal(sim_set_noexpect(exp, ""), SCPE_OK);
    assert_int_equal(exp->size, 0);
}

/* Verify HALTAFTER rules reject concurrent siblings and CLEARALL clears all. */
static void test_expect_rule_restrictions_and_clearall_match_behavior(
    void **state)
{
    struct scp_expect_fixture *fixture = *state;

    assert_int_equal(
        sim_exp_set(&fixture->exp, "\"A\"", 0, 0, EXP_TYP_PERSIST, NULL),
        SCPE_OK);
    assert_int_equal(
        SCPE_BARE_STATUS(
            sim_exp_set(&fixture->exp, "\"B\"", 0, 1, 0, NULL)),
        SCPE_ARG);

    assert_int_equal(sim_exp_clrall(&fixture->exp), SCPE_OK);
    assert_int_equal(sim_exp_set(&fixture->exp, "\"A\"", 0, 0,
                                 EXP_TYP_PERSIST, NULL),
                     SCPE_OK);
    assert_int_equal(sim_exp_set(&fixture->exp, "\"B\"", 0, 0,
                                 EXP_TYP_CLEARALL, NULL),
                     SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'B'), SCPE_OK);
    assert_int_equal(fixture->exp.size, 0);
}

/* Verify SHOW reports filtered no-match errors for expect rules. */
static void test_sim_exp_show_reports_when_no_rules_match_filter(void **state)
{
    struct scp_expect_fixture *fixture = *state;
    FILE *stream;
    char *text;
    size_t size;

    assert_int_equal(
        sim_exp_set(&fixture->exp, "\"A\"", 0, 0, EXP_TYP_PERSIST, NULL),
        SCPE_OK);

    stream = tmpfile();
    assert_non_null(stream);
    assert_int_equal(sim_exp_show(stream, &fixture->exp, "\"NOPE\""),
                     SCPE_ARG);
    assert_int_equal(simh_test_read_stream(stream, &text, &size), 0);
    assert_non_null(strstr(text, "No Rules match '\"NOPE\"'"));
    free(text);
    fclose(stream);
}

/* Verify exact-string matching works when the buffer wraps mid-pattern. */
static void test_sim_exp_check_matches_across_buffer_wrap(void **state)
{
    struct scp_expect_fixture *fixture = *state;

    assert_int_equal(
        sim_exp_set(&fixture->exp, "\"ABCDE\"", 0, 0, EXP_TYP_PERSIST, NULL),
        SCPE_OK);

    assert_int_equal(sim_exp_check(&fixture->exp, 'x'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'x'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'A'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'B'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'C'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'D'), SCPE_OK);
    assert_int_equal(fixture->exp.buf_ins, 0);
    assert_int_equal(sim_exp_check(&fixture->exp, 'E'), SCPE_OK);

    assert_string_equal(getenv("_EXPECT_MATCH_PATTERN"), "\"ABCDE\"");
}

/* Verify regex buffers slide instead of wrapping when they fill. */
static void test_sim_exp_check_slides_regex_buffers_when_full(void **state)
{
    struct scp_expect_fixture *fixture = *state;
    size_t i;

    assert_int_equal(
        sim_exp_set(&fixture->exp, "/never-matches/", 0, 0,
                    EXP_TYP_REGEX | EXP_TYP_PERSIST, NULL),
        SCPE_OK);
    assert_int_equal(fixture->exp.buf_size, 1025);

    for (i = 0; i < fixture->exp.buf_size; ++i)
        assert_int_equal(sim_exp_check(&fixture->exp, 'a'), SCPE_OK);

    assert_int_equal(fixture->exp.buf_ins, 513);
    assert_int_equal(fixture->exp.buf_data, fixture->exp.buf_ins);
}

/* Verify expect matches activate the internal unit and identify it. */
static void test_expect_matches_schedule_internal_expect_unit(void **state)
{
    struct scp_expect_fixture *fixture = *state;

    assert_false(sim_expect_is_unit(&fixture->unit));
    assert_true(sim_expect_is_unit(&sim_expect_dev.units[0]));
    assert_int_equal(
        sim_exp_set(&fixture->exp, "\"HI\"", 0, 0, 0, NULL), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'H'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'I'), SCPE_OK);
    assert_int_equal(SCPE_BARE_STATUS(sim_process_event()), SCPE_EXPECT);
}

/* Verify expect rules can schedule a microsecond-based stop delay. */
static void test_expect_time_rules_schedule_microsecond_based_stop(
    void **state)
{
    struct scp_expect_fixture *fixture = *state;
    int32 expected_delay;
    int32 scheduled_delay;

    expected_delay =
        (int32)((sim_timer_inst_per_sec() * 5000) / 1000000.0);

    assert_int_equal(sim_exp_set(&fixture->exp, "\"GO\"", 0, 5000,
                                 EXP_TYP_TIME, NULL),
                     SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'G'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'O'), SCPE_OK);
    assert_true(sim_is_active(&sim_expect_dev.units[0]));
    scheduled_delay = sim_activate_time(&sim_expect_dev.units[0]);
    assert_true(scheduled_delay >= expected_delay);
    assert_true(scheduled_delay > 0);
}

/* Verify expect helpers handle empty contexts and expose descriptions. */
static void test_expect_helpers_handle_empty_contexts(void **state)
{
    struct scp_expect_fixture *fixture = *state;
    FILE *stream;

    assert_string_equal(sim_expect_dev.description(&sim_expect_dev),
                        "Expect facility");
    assert_int_equal(sim_exp_check(&fixture->exp, 'X'), SCPE_OK);

    stream = tmpfile();
    assert_non_null(stream);
    assert_int_equal(sim_exp_showall(stream, &fixture->exp), SCPE_OK);
    fclose(stream);
}

/* Verify the SHOW SEND wrapper accepts the console path and rejects extras. */
static void test_sim_show_send_wrapper_handles_console_arguments(void **state)
{
    FILE *stream;

    (void)state;

    stream = tmpfile();
    assert_non_null(stream);
    assert_int_equal(sim_show_send(stream, NULL, NULL, 0, ""), SCPE_OK);
    fclose(stream);

    stream = tmpfile();
    assert_non_null(stream);
    assert_int_equal(sim_show_send(stream, NULL, NULL, 0, "extra"),
                     SCPE_2MARG);
    fclose(stream);
}

/* Verify the SHOW EXPECT wrapper accepts quoted filters and rejects extras. */
static void test_sim_show_expect_wrapper_handles_console_arguments(void **state)
{
    EXPECT *exp;
    FILE *stream;

    (void)state;

    exp = sim_cons_get_expect();
    assert_int_equal(sim_exp_set(exp, "\"A\"", 0, 0, EXP_TYP_PERSIST, NULL),
                     SCPE_OK);

    stream = tmpfile();
    assert_non_null(stream);
    assert_int_equal(sim_show_expect(stream, NULL, NULL, 0, "\"A\""),
                     SCPE_OK);
    fclose(stream);

    stream = tmpfile();
    assert_non_null(stream);
    assert_int_equal(sim_show_expect(stream, NULL, NULL, 0, "\"A\" extra"),
                     SCPE_2MARG);
    fclose(stream);
}

/* Verify malformed NOEXPECT inputs are rejected through parser helpers. */
static void test_sim_set_noexpect_rejects_invalid_argument_forms(void **state)
{
    EXPECT *exp;

    (void)state;

    exp = sim_cons_get_expect();
    assert_int_equal(
        SCPE_BARE_STATUS(sim_set_noexpect(exp, "plain")), SCPE_ARG);
    assert_int_equal(sim_set_noexpect(exp, "\"A\" extra"), SCPE_2MARG);
    assert_int_equal(SCPE_BARE_STATUS(expect_cmd(0, "plain")), SCPE_ARG);
}

/* Verify EXPECT parser rejects missing input and non-regex -i usage. */
static void test_expect_parsers_reject_missing_input_and_bad_switches(
    void **state)
{
    EXPECT *exp;

    (void)state;

    exp = sim_cons_get_expect();
    assert_int_equal(sim_set_expect(exp, ""), SCPE_2FARG);
    assert_int_equal(
        SCPE_BARE_STATUS(
            sim_exp_set(exp, "\"A\"", 0, 0, EXP_TYP_REGEX_I, NULL)),
        SCPE_ARG);
}

/* Verify invalid literal strings are rejected for exact-match rules. */
static void test_sim_exp_set_rejects_invalid_quoted_match_string(void **state)
{
    struct scp_expect_fixture *fixture = *state;

    assert_int_equal(
        SCPE_BARE_STATUS(
            sim_exp_set(&fixture->exp, "\"\\q\"", 0, 0, 0, NULL)),
        SCPE_ARG);
}

/* Verify SEND state compacts pending bytes and honors future timing. */
static void test_send_state_compacts_pending_bytes_and_honors_timing(
    void **state)
{
    struct scp_expect_fixture *fixture = *state;
    static uint8 payload[] = {'A', 'B', 'C'};
    t_stat stat;

    assert_int_equal(
        sim_send_input(&fixture->snd, payload, sizeof(payload), 5, 7),
        SCPE_OK);
    assert_true(sim_send_poll_data(&fixture->snd, &stat));
    assert_int_equal(stat, SCPE_OK);

    fixture->snd.next_time = sim_gtime();
    assert_true(sim_send_poll_data(&fixture->snd, &stat));
    assert_int_equal(stat, 'A' | SCPE_KFLAG);
    assert_int_equal(fixture->snd.extoff, 1);

    fixture->snd.next_time = sim_gtime();
    assert_int_equal(
        sim_send_input(&fixture->snd, payload, sizeof(payload), 0, 0),
        SCPE_OK);
    assert_int_equal(fixture->snd.extoff, 0);
    assert_int_equal(fixture->snd.insoff, 5);
    assert_memory_equal(fixture->snd.buffer, "BCABC", fixture->snd.insoff);
}

/* Verify SHOW SEND renders pending timing and invalid env vars fallback. */
static void test_sim_show_send_input_renders_timing_and_env_fallback(
    void **state)
{
    SEND *snd;
    static uint8 payload[] = {'A'};
    FILE *stream;
    char *text;
    size_t size;

    (void)state;

    snd = sim_cons_get_send();
    setenv("SIM_SEND_DELAY_CONSOLE", "bogus", 1);
    setenv("SIM_SEND_AFTER_CONSOLE", "bogus", 1);
    assert_int_equal(
        sim_send_input(snd, payload, sizeof(payload), 5000, 2000000), SCPE_OK);

    stream = tmpfile();
    assert_non_null(stream);
    assert_int_equal(sim_show_send_input(stream, snd), SCPE_OK);
    assert_int_equal(simh_test_read_stream(stream, &text, &size), 0);
    assert_non_null(strstr(text, "pending input Data"));
    assert_non_null(strstr(text, "first character"));
    assert_non_null(strstr(text, "between characters"));
    assert_non_null(
        strstr(text, "Default delay before first character input is 1000"));
    assert_non_null(
        strstr(text, "Default delay between character input is 1000"));
    free(text);
    fclose(stream);
}

/* Verify SHOW SEND reports distinct default after and delay values. */
static void test_sim_show_send_input_reports_distinct_default_values(
    void **state)
{
    SEND *snd;
    FILE *stream;
    char *text;
    size_t size;

    (void)state;

    snd = sim_cons_get_send();
    setenv("SIM_SEND_DELAY_CONSOLE", "7", 1);
    setenv("SIM_SEND_AFTER_CONSOLE", "11", 1);

    stream = tmpfile();
    assert_non_null(stream);
    assert_int_equal(sim_show_send_input(stream, snd), SCPE_OK);
    assert_int_equal(simh_test_read_stream(stream, &text, &size), 0);
    assert_non_null(
        strstr(text, "Default delay before first character input is 11"));
    assert_non_null(
        strstr(text, "Default delay between character input is 7"));
    free(text);
    fclose(stream);
}

/* Verify regex export clears optional capture groups that did not match. */
static void test_regex_optional_group_unsets_unmatched_capture_group(
    void **state)
{
    struct scp_expect_fixture *fixture = *state;
    const char *group1;

    assert_int_equal(sim_exp_set(&fixture->exp, "/ab(c)?/", 0, 0,
                                 EXP_TYP_REGEX, NULL),
                     SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'a'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'b'), SCPE_OK);

    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_0"), "ab");
    group1 = getenv("_EXPECT_MATCH_GROUP_1");
    assert_true((NULL == group1) || (group1[0] == '\0'));
}

/* Verify regex checks flatten wrapped buffers with embedded NUL bytes. */
static void test_regex_matching_flattens_embedded_nul_buffer_segments(
    void **state)
{
    struct scp_expect_fixture *fixture = *state;

    assert_int_equal(sim_exp_set(&fixture->exp, "/AB/", 0, 0,
                                 EXP_TYP_REGEX | EXP_TYP_PERSIST, NULL),
                     SCPE_OK);
    assert_int_equal(sim_exp_set(&fixture->exp, "/ZZ/", 0, 0,
                                 EXP_TYP_REGEX | EXP_TYP_PERSIST, NULL),
                     SCPE_OK);

    assert_int_equal(sim_exp_check(&fixture->exp, 'A'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, '\0'), SCPE_OK);
    assert_int_equal(sim_exp_check(&fixture->exp, 'B'), SCPE_OK);

    assert_string_equal(getenv("_EXPECT_MATCH_PATTERN"), "/AB/");
    assert_string_equal(getenv("_EXPECT_MATCH_GROUP_0"), "AB");
}

/* Verify SHOW can render a decorated expect rule through the match filter. */
static void test_sim_exp_show_renders_decorated_rule_details(void **state)
{
    EXPECT *exp;
    FILE *stream;
    char *text;
    size_t size;

    (void)state;

    exp = sim_cons_get_expect();
    setenv("SIM_EXPECT_HALTAFTER_CONSOLE", "1", 1);
    assert_int_equal(sim_exp_set(exp, "/A(B)?/", 2, 9,
                                 EXP_TYP_PERSIST | EXP_TYP_CLEARALL |
                                     EXP_TYP_REGEX | EXP_TYP_REGEX_I,
                                 "echo done"),
                     SCPE_OK);

    stream = tmpfile();
    assert_non_null(stream);
    assert_int_equal(sim_exp_show(stream, exp, "/A(B)?/"), SCPE_OK);
    assert_int_equal(simh_test_read_stream(stream, &text, &size), 0);
    assert_non_null(strstr(text, "EXPECT -p -c -r -i"));
    assert_non_null(strstr(text, "HALTAFTER=9"));
    assert_non_null(strstr(text, "[2]"));
    assert_non_null(strstr(text, "echo done"));
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
        cmocka_unit_test_setup_teardown(test_show_helpers_render_pending_state,
                                        setup_scp_expect_fixture,
                                        teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_send_cmd_sets_console_defaults_and_queue_timing,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_send_cmd_rejects_invalid_argument_forms,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_send_cmd_time_switch_converts_usec_to_instructions,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_expect_parses_repeat_haltafter_and_action,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_expect_cmd_parses_switches_and_rejects_bad_input,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_noexpect_removes_specific_rules_and_all,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_expect_rule_restrictions_and_clearall_match_behavior,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_exp_show_reports_when_no_rules_match_filter,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_exp_check_matches_across_buffer_wrap,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_exp_check_slides_regex_buffers_when_full,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_expect_matches_schedule_internal_expect_unit,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_show_send_wrapper_handles_console_arguments,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_show_expect_wrapper_handles_console_arguments,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_set_noexpect_rejects_invalid_argument_forms,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_expect_parsers_reject_missing_input_and_bad_switches,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_exp_set_rejects_invalid_quoted_match_string,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_send_state_compacts_pending_bytes_and_honors_timing,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_show_send_input_renders_timing_and_env_fallback,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_show_send_input_reports_distinct_default_values,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_regex_optional_group_unsets_unmatched_capture_group,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_regex_matching_flattens_embedded_nul_buffer_segments,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_exp_show_renders_decorated_rule_details,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_expect_time_rules_schedule_microsecond_based_stop,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
        cmocka_unit_test_setup_teardown(
            test_expect_helpers_handle_empty_contexts,
            setup_scp_expect_fixture, teardown_scp_expect_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
