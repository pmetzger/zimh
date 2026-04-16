#include <stddef.h>
#include <setjmp.h>
#include <string.h>

#include <cmocka.h>

#include "scp.h"
#include "scp_parse.h"

static void test_get_glyph_uppercases_and_skips_spaces(void **state)
{
    char token[CBUFSIZE];
    const char *rest;

    (void)state;

    rest = get_glyph("alpha  beta", token, 0);

    assert_string_equal(token, "ALPHA");
    assert_string_equal(rest, "beta");
}

static void test_get_glyph_nc_preserves_case(void **state)
{
    char token[CBUFSIZE];
    const char *rest;

    (void)state;

    rest = get_glyph_nc("MiXeD,next", token, ',');

    assert_string_equal(token, "MiXeD");
    assert_string_equal(rest, "next");
}

static void test_get_glyph_quoted_keeps_quoted_token_intact(void **state)
{
    char token[CBUFSIZE];
    const char *rest;

    (void)state;

    rest = get_glyph_quoted("\"hello world\" tail", token, 0);

    assert_string_equal(token, "\"hello world\"");
    assert_string_equal(rest, "tail");
}

static void test_get_glyph_cmd_handles_bang_prefix(void **state)
{
    char token[CBUFSIZE];
    const char *rest;

    (void)state;

    rest = get_glyph_cmd("!echo test", token);

    assert_string_equal(token, "!");
    assert_string_equal(rest, "echo test");
}

static void test_get_switches_parses_bitmask_and_number(void **state)
{
    int32 sw;
    int32 number;

    (void)state;

    assert_int_equal(get_switches("-qr", &sw, &number), SW_BITMASK);
    assert_int_equal(sw, SWMASK('Q') | SWMASK('R'));

    assert_int_equal(get_switches("-16", &sw, &number), SW_NUMBER);
    assert_int_equal(number, 16);
}

static void test_get_switches_rejects_invalid_switches(void **state)
{
    int32 sw;
    int32 number;

    (void)state;

    assert_int_equal(get_switches("-q1", &sw, &number), SW_ERROR);
    assert_int_equal(get_switches("-?", &sw, &number), SW_ERROR);
}

static void test_get_sim_sw_consumes_switch_tokens(void **state)
{
    const char *rest;

    (void)state;

    sim_switches = 0;
    sim_switch_number = 0;

    rest = get_sim_sw("-qr -16 command tail");

    assert_non_null(rest);
    assert_int_equal(sim_switches, SWMASK('Q') | SWMASK('R'));
    assert_int_equal(sim_switch_number, 16);
    assert_string_equal(rest, "command tail");
}

static void test_get_sim_sw_returns_null_on_invalid_switches(void **state)
{
    (void)state;

    sim_switches = 0;
    sim_switch_number = 0;

    assert_null(get_sim_sw("-q1 command"));
    assert_int_equal(sim_switches, 0);
    assert_int_equal(sim_switch_number, 0);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_get_glyph_uppercases_and_skips_spaces),
        cmocka_unit_test(test_get_glyph_nc_preserves_case),
        cmocka_unit_test(test_get_glyph_quoted_keeps_quoted_token_intact),
        cmocka_unit_test(test_get_glyph_cmd_handles_bang_prefix),
        cmocka_unit_test(test_get_switches_parses_bitmask_and_number),
        cmocka_unit_test(test_get_switches_rejects_invalid_switches),
        cmocka_unit_test(test_get_sim_sw_consumes_switch_tokens),
        cmocka_unit_test(test_get_sim_sw_returns_null_on_invalid_switches),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
