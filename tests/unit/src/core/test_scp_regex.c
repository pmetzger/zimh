#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "scp.h"
#include "test_simh_personality.h"
#include "test_support.h"

static int setup_scp_regex_fixture(void **state)
{
    (void)state;

    unsetenv("SIM_REGEX_TYPE");
    return 0;
}

static int teardown_scp_regex_fixture(void **state)
{
    (void)state;

    unsetenv("SIM_REGEX_TYPE");
    return 0;
}

/* Verify the backend name/version helpers reflect the compiled support. */
static void test_sim_regex_backend_helpers_match_build_configuration(void **state)
{
    const char *backend;
    const char *version;

    (void)state;

    backend = sim_regex_backend_name();
    version = sim_regex_backend_version();

#if defined(HAVE_PCRE2_H)
    assert_string_equal(backend, "PCRE2");
    assert_non_null(version);
    assert_true(version[0] != '\0');
#else
    assert_null(backend);
    assert_null(version);
#endif
}

/* Verify publishing regex support exports the expected environment value. */
static void test_sim_publish_regex_environment_sets_expected_value(void **state)
{
    const char *backend;

    (void)state;

    sim_publish_regex_environment();
    backend = sim_regex_backend_name();

    if (backend)
        assert_string_equal(getenv("SIM_REGEX_TYPE"), backend);
    else
        assert_null(getenv("SIM_REGEX_TYPE"));
}

/* Verify the regex support formatter describes the current build mode. */
static void test_sim_fprint_regex_support_reports_current_backend(void **state)
{
    const char *backend;
    const char *version;
    FILE *stream;
    char *text;
    size_t size;

    (void)state;

    backend = sim_regex_backend_name();
    version = sim_regex_backend_version();
    stream = tmpfile();
    assert_non_null(stream);

    sim_fprint_regex_support(stream);
    assert_int_equal(simh_test_read_stream(stream, &text, &size), 0);
    assert_non_null(text);

    if (backend) {
        assert_non_null(strstr(text, backend));
        assert_non_null(strstr(text, "EXPECT commands"));
        if (version && *version)
            assert_non_null(strstr(text, version));
    } else
        assert_non_null(strstr(text, "No RegEx support"));

    free(text);
    fclose(stream);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_sim_regex_backend_helpers_match_build_configuration,
            setup_scp_regex_fixture, teardown_scp_regex_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_publish_regex_environment_sets_expected_value,
            setup_scp_regex_fixture, teardown_scp_regex_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_fprint_regex_support_reports_current_backend,
            setup_scp_regex_fixture, teardown_scp_regex_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
