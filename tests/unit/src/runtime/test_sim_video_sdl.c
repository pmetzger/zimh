#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include <cmocka.h>

#include "sim_video.h"

static void use_dummy_sdl_drivers(void)
{
#if defined(_WIN32)
    _putenv_s("SDL_AUDIODRIVER", "dummy");
    _putenv_s("SDL_VIDEODRIVER", "dummy");
#else
    setenv("SDL_AUDIODRIVER", "dummy", 1);
    setenv("SDL_VIDEODRIVER", "dummy", 1);
#endif
}

static void test_screenshot_reports_surface_allocation_failure(void **state)
{
    const char *path = "zimh-unit-video-screenshot.bmp";
    VID_DISPLAY *display = NULL;

    (void)state;

    use_dummy_sdl_drivers();
    remove(path);

    assert_int_equal(vid_open_window(&display, NULL, "screenshot", 16, 16, 0),
                     SCPE_OK);
    assert_non_null(display);

    assert_int_equal(vid_screenshot(path), SCPE_MEM);

    remove(path);
    assert_int_equal(vid_close_all(), SCPE_OK);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_screenshot_reports_surface_allocation_failure),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
