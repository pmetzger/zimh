#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include "sim_dynstr.h"

static int simh_test_dynstr_realloc_calls = 0;
static int simh_test_dynstr_fail_after = -1;

/* Fail allocation after the configured number of successful calls. */
static void *simh_test_dynstr_realloc_fail(void *ptr, size_t size)
{
    ++simh_test_dynstr_realloc_calls;
    if (simh_test_dynstr_fail_after == 0)
        return NULL;
    if (simh_test_dynstr_fail_after > 0)
        --simh_test_dynstr_fail_after;
    return realloc(ptr, size);
}

/* Reset hooks and counters so each dynamic-string case is isolated. */
static int setup_sim_dynstr_fixture(void **state)
{
    (void)state;

    simh_test_dynstr_realloc_calls = 0;
    simh_test_dynstr_fail_after = -1;
    sim_dynstr_reset_test_hooks();
    return 0;
}

/* Clear the allocator hook after each case. */
static int teardown_sim_dynstr_fixture(void **state)
{
    (void)state;

    sim_dynstr_reset_test_hooks();
    return 0;
}

/* Verify init yields an empty logical string without storage. */
static void test_sim_dynstr_init_yields_empty_string(void **state)
{
    sim_dynstr_t ds;

    (void)state;

    sim_dynstr_init(&ds);
    assert_null(ds.buf);
    assert_int_equal(ds.len, 0);
    assert_int_equal(ds.cap, 0);
    assert_string_equal(sim_dynstr_cstr(&ds), "");
}

/* Verify append creates storage and preserves a trailing NUL. */
static void test_sim_dynstr_append_builds_string(void **state)
{
    sim_dynstr_t ds;

    (void)state;

    sim_dynstr_init(&ds);
    assert_true(sim_dynstr_append(&ds, "alpha"));
    assert_non_null(ds.buf);
    assert_string_equal(sim_dynstr_cstr(&ds), "alpha");
    assert_int_equal(ds.len, 5);
    assert_true(ds.cap >= 6);
    sim_dynstr_free(&ds);
}

/* Verify append_ch extends an existing string by one character. */
static void test_sim_dynstr_append_ch_extends_string(void **state)
{
    sim_dynstr_t ds;

    (void)state;

    sim_dynstr_init(&ds);
    assert_true(sim_dynstr_append(&ds, "ab"));
    assert_true(sim_dynstr_append_ch(&ds, 'c'));
    assert_string_equal(sim_dynstr_cstr(&ds), "abc");
    assert_int_equal(ds.len, 3);
    sim_dynstr_free(&ds);
}

/* Verify growth preserves earlier text while expanding capacity. */
static void test_sim_dynstr_grows_and_preserves_contents(void **state)
{
    sim_dynstr_t ds;
    char long_text[40];

    (void)state;

    memset(long_text, 'x', sizeof(long_text) - 1);
    long_text[sizeof(long_text) - 1] = '\0';
    sim_dynstr_init(&ds);
    assert_true(sim_dynstr_append(&ds, long_text));
    assert_true(sim_dynstr_append(&ds, long_text));
    assert_string_equal(sim_dynstr_cstr(&ds),
                        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
                        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    assert_true(ds.cap > 16);
    sim_dynstr_free(&ds);
}

/* Verify append failure preserves the prior contents and capacity. */
static void test_sim_dynstr_append_failure_preserves_state(void **state)
{
    sim_dynstr_t ds;
    size_t old_cap;

    (void)state;

    sim_dynstr_init(&ds);
    assert_true(sim_dynstr_append(&ds, "alpha"));
    old_cap = ds.cap;
    simh_test_dynstr_fail_after = 0;
    sim_dynstr_set_test_realloc_hook(simh_test_dynstr_realloc_fail);

    assert_false(sim_dynstr_append(&ds, "this string forces growth"));
    assert_int_equal(simh_test_dynstr_realloc_calls, 1);
    assert_string_equal(sim_dynstr_cstr(&ds), "alpha");
    assert_int_equal(ds.len, 5);
    assert_int_equal(ds.cap, old_cap);
    sim_dynstr_free(&ds);
}

/* Verify append_ch failure also preserves the earlier text. */
static void test_sim_dynstr_append_ch_failure_preserves_state(void **state)
{
    sim_dynstr_t ds;
    char full_text[32];

    (void)state;

    memset(full_text, 'a', sizeof(full_text) - 1);
    full_text[sizeof(full_text) - 1] = '\0';
    sim_dynstr_init(&ds);
    assert_true(sim_dynstr_append(&ds, full_text));
    simh_test_dynstr_fail_after = 0;
    sim_dynstr_set_test_realloc_hook(simh_test_dynstr_realloc_fail);

    assert_false(sim_dynstr_append_ch(&ds, 'q'));
    assert_int_equal(simh_test_dynstr_realloc_calls, 1);
    assert_string_equal(sim_dynstr_cstr(&ds), full_text);
    assert_int_equal(ds.len, 31);
    sim_dynstr_free(&ds);
}

/* Verify free releases storage and resets the struct to the empty state. */
static void test_sim_dynstr_free_resets_state(void **state)
{
    sim_dynstr_t ds;

    (void)state;

    sim_dynstr_init(&ds);
    assert_true(sim_dynstr_append(&ds, "alpha"));
    sim_dynstr_free(&ds);
    assert_null(ds.buf);
    assert_int_equal(ds.len, 0);
    assert_int_equal(ds.cap, 0);
    assert_string_equal(sim_dynstr_cstr(&ds), "");
}

/* Verify resetting hooks restores the default allocator path. */
static void test_sim_dynstr_reset_restores_default_allocator(void **state)
{
    sim_dynstr_t ds;

    (void)state;

    sim_dynstr_init(&ds);
    simh_test_dynstr_fail_after = 0;
    sim_dynstr_set_test_realloc_hook(simh_test_dynstr_realloc_fail);
    sim_dynstr_reset_test_hooks();

    assert_true(sim_dynstr_append(&ds, "alpha"));
    assert_string_equal(sim_dynstr_cstr(&ds), "alpha");
    sim_dynstr_free(&ds);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_sim_dynstr_init_yields_empty_string, setup_sim_dynstr_fixture,
            teardown_sim_dynstr_fixture),
        cmocka_unit_test_setup_teardown(test_sim_dynstr_append_builds_string,
                                        setup_sim_dynstr_fixture,
                                        teardown_sim_dynstr_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_dynstr_append_ch_extends_string, setup_sim_dynstr_fixture,
            teardown_sim_dynstr_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_dynstr_grows_and_preserves_contents,
            setup_sim_dynstr_fixture, teardown_sim_dynstr_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_dynstr_append_failure_preserves_state,
            setup_sim_dynstr_fixture, teardown_sim_dynstr_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_dynstr_append_ch_failure_preserves_state,
            setup_sim_dynstr_fixture, teardown_sim_dynstr_fixture),
        cmocka_unit_test_setup_teardown(test_sim_dynstr_free_resets_state,
                                        setup_sim_dynstr_fixture,
                                        teardown_sim_dynstr_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_dynstr_reset_restores_default_allocator,
            setup_sim_dynstr_fixture, teardown_sim_dynstr_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
