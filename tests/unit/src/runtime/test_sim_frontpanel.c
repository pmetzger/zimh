#include <stddef.h>
#include <stdint.h>

#include "test_cmocka.h"

#include "sim_time.h"

#include "sim_frontpanel.c"

static int simh_test_frontpanel_clock_calls = 0;
static int simh_test_frontpanel_last_clock_id = -1;

/* Return a deterministic timestamp through the shared time wrapper. */
static int simh_test_frontpanel_clock(int clock_id, struct timespec *tp)
{
    ++simh_test_frontpanel_clock_calls;
    simh_test_frontpanel_last_clock_id = clock_id;
    if (tp != NULL)
        *tp = (struct timespec){.tv_sec = 1234, .tv_nsec = 567800000L};
    return 0;
}

/* Reset the shared clock hook and create the thread-name key used by debug. */
static int setup_sim_frontpanel_fixture(void **state)
{
    (void)state;

    simh_test_frontpanel_clock_calls = 0;
    simh_test_frontpanel_last_clock_id = -1;
    sim_time_reset_test_hooks();
    assert_int_equal(pthread_key_create(&panel_thread_id, NULL), 0);
    return 0;
}

/* Clear the debug thread-name key and shared clock hook. */
static int teardown_sim_frontpanel_fixture(void **state)
{
    (void)state;

    pthread_key_delete(panel_thread_id);
    sim_time_reset_test_hooks();
    return 0;
}

/* Verify frontpanel debug timestamps come from the shared time wrapper. */
static void test_frontpanel_debug_uses_shared_clock_wrapper(void **state)
{
    PANEL panel = {0};
    char output[128];

    (void)state;

    panel.Debug = tmpfile();
    assert_non_null(panel.Debug);
    panel.debug = DBG_APP;
    sim_time_set_test_hooks(simh_test_frontpanel_clock, NULL);

    sim_panel_debug(&panel, "frontpanel event");

    fflush(panel.Debug);
    rewind(panel.Debug);
    assert_non_null(fgets(output, sizeof(output), panel.Debug));
    fclose(panel.Debug);

    assert_int_equal(simh_test_frontpanel_clock_calls, 1);
    assert_int_equal(simh_test_frontpanel_last_clock_id, CLOCK_REALTIME);
    assert_string_equal(output, "1234.567 CPU: frontpanel event\n");
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_frontpanel_debug_uses_shared_clock_wrapper,
            setup_sim_frontpanel_fixture, teardown_sim_frontpanel_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
