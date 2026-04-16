#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "sim_defs.h"
#include "scp.h"
#include "sim_card.h"
#include "test_simh_personality.h"
#include "test_support.h"

struct sim_card_fixture {
    char temp_dir[1024];
    char input_path[1024];
    char output_path[1024];
    UNIT reader_unit;
    UNIT punch_unit;
    DEVICE reader_dev;
    DEVICE punch_dev;
};

static void init_card_device(DEVICE *device, UNIT *unit, const char *dev_name,
                             const char *unit_name, uint32 flags)
{
    memset(device, 0, sizeof(*device));
    memset(unit, 0, sizeof(*unit));

    unit->flags = UNIT_ATTABLE | flags;
    unit->uname = (char *)unit_name;
    unit->dptr = device;

    device->name = dev_name;
    device->units = unit;
    device->numunits = 1;
    device->flags = DEV_CARD;
}

static int make_path(char *buffer, size_t buffer_size, const char *dir_path,
                     const char *leaf_name)
{
    int length;

    length = snprintf(buffer, buffer_size, "%s/%s", dir_path, leaf_name);
    if (length < 0 || (size_t)length >= buffer_size) {
        return -1;
    }

    return 0;
}

static int setup_sim_card_fixture(void **state)
{
    struct sim_card_fixture *fixture;
    DEVICE *devices[3];

    fixture = calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    assert_int_equal(simh_test_make_temp_dir(fixture->temp_dir,
                                             sizeof(fixture->temp_dir),
                                             "sim-card"),
                     0);
    assert_int_equal(make_path(fixture->input_path, sizeof(fixture->input_path),
                               fixture->temp_dir, "input.deck"),
                     0);
    assert_int_equal(make_path(fixture->output_path,
                               sizeof(fixture->output_path), fixture->temp_dir,
                               "output.deck"),
                     0);

    init_card_device(&fixture->reader_dev, &fixture->reader_unit, "RDR", "RDR",
                     UNIT_RO | MODE_TEXT);
    init_card_device(&fixture->punch_dev, &fixture->punch_unit, "PUN", "PUN",
                     MODE_TEXT);

    simh_test_reset_simulator_state();
    assert_int_equal(simh_test_set_sim_name("simbase-unit-sim-card"), 0);

    devices[0] = &fixture->reader_dev;
    devices[1] = &fixture->punch_dev;
    devices[2] = NULL;
    assert_int_equal(simh_test_set_devices(devices), 0);

    *state = fixture;
    return 0;
}

static int teardown_sim_card_fixture(void **state)
{
    struct sim_card_fixture *fixture = *state;

    if ((fixture->reader_unit.flags & UNIT_ATT) != 0) {
        assert_int_equal(sim_card_detach(&fixture->reader_unit), SCPE_OK);
    }
    if ((fixture->punch_unit.flags & UNIT_ATT) != 0) {
        assert_int_equal(sim_card_detach(&fixture->punch_unit), SCPE_OK);
    }

    assert_int_equal(simh_test_remove_path(fixture->temp_dir), 0);
    free(fixture);
    *state = NULL;
    return 0;
}

static void test_sim_card_text_deck_handles_crlf_and_tabs(void **state)
{
    static const char deck_text[] = "A\tB\r\nWORLD\n~\n";
    static const char expected_output[] = "A       B\nWORLD\n";
    struct sim_card_fixture *fixture = *state;
    uint16 image[80];
    void *output_data;
    size_t output_size;

    assert_int_equal(simh_test_write_file(fixture->input_path, deck_text,
                                          sizeof(deck_text) - 1),
                     0);

    assert_int_equal(
        sim_card_attach(&fixture->reader_unit, fixture->input_path), SCPE_OK);
    assert_int_equal(
        sim_card_attach(&fixture->punch_unit, fixture->output_path), SCPE_OK);

    assert_int_equal(sim_card_input_hopper_count(&fixture->reader_unit), 2);

    assert_int_equal(sim_read_card(&fixture->reader_unit, image), CDSE_OK);
    assert_int_equal(sim_punch_card(&fixture->punch_unit, image), CDSE_OK);
    assert_int_equal(sim_card_input_hopper_count(&fixture->reader_unit), 1);

    assert_int_equal(sim_read_card(&fixture->reader_unit, image), CDSE_OK);
    assert_int_equal(sim_punch_card(&fixture->punch_unit, image), CDSE_OK);
    assert_int_equal(sim_card_input_hopper_count(&fixture->reader_unit), 0);
    assert_true(sim_card_eof(&fixture->reader_unit));

    assert_int_equal(sim_read_card(&fixture->reader_unit, image), CDSE_EOF);

    assert_int_equal(sim_card_detach(&fixture->punch_unit), SCPE_OK);

    assert_int_equal(
        simh_test_read_file(fixture->output_path, &output_data, &output_size),
        0);
    assert_int_equal(output_size, strlen(expected_output));
    assert_memory_equal(output_data, expected_output, output_size);
    free(output_data);
}

static void test_sim_card_reports_text_conversion_errors(void **state)
{
    static const unsigned char deck_bytes[] = {'A', 0x01, 'B', '\n', '~', '\n'};
    struct sim_card_fixture *fixture = *state;
    uint16 image[80];

    assert_int_equal(simh_test_write_file(fixture->input_path, deck_bytes,
                                          sizeof(deck_bytes)),
                     0);

    assert_int_equal(
        sim_card_attach(&fixture->reader_unit, fixture->input_path), SCPE_OK);
    assert_int_equal(sim_card_input_hopper_count(&fixture->reader_unit), 1);

    assert_int_equal(sim_read_card(&fixture->reader_unit, image), CDSE_ERROR);
    assert_true(sim_card_eof(&fixture->reader_unit));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_sim_card_text_deck_handles_crlf_and_tabs,
            setup_sim_card_fixture, teardown_sim_card_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_card_reports_text_conversion_errors,
            setup_sim_card_fixture, teardown_sim_card_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
