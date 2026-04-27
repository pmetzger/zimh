#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "sim_scsi.h"
#include "test_support.h"

struct scsi_message_case {
    uint8 message;
    SCSI_BUS bus;
};

static DEVICE test_scsi_device = {
    .name = "SCSITEST",
};

static void setup_message_bus(SCSI_BUS *bus)
{
    memset(bus, 0, sizeof(*bus));
    bus->dptr = &test_scsi_device;
    assert_int_equal(scsi_init(bus, 16), SCPE_OK);
    scsi_reset(bus);

    bus->initiator = 6;
    bus->target = 4;
    bus->phase = SCSI_MSGO;
    bus->req = TRUE;
}

static void teardown_message_bus(SCSI_BUS *bus)
{
    free(bus->buf);
}

static void write_scsi_message(void *context)
{
    struct scsi_message_case *message_case = context;
    uint8 message = message_case->message;

    assert_int_equal(scsi_write(&message_case->bus, &message, 1), 1);
}

static void assert_standard_message_is_accepted_silently(uint8 message)
{
    struct scsi_message_case message_case = {
        .message = message,
    };
    char *output = NULL;
    size_t output_size = 0;

    setup_message_bus(&message_case.bus);

    assert_int_equal(simh_test_capture_stdout(write_scsi_message,
                                              &message_case, &output,
                                              &output_size),
                     0);
    assert_string_equal(output, "");
    assert_int_equal(output_size, 0);
    assert_int_equal(message_case.bus.phase, SCSI_CMD);
    assert_int_equal(message_case.bus.initiator, 6);
    assert_int_equal(message_case.bus.target, 4);

    free(output);
    teardown_message_bus(&message_case.bus);
}

static void assert_message_disconnects_silently(uint8 message)
{
    struct scsi_message_case message_case = {
        .message = message,
    };
    char *output = NULL;
    size_t output_size = 0;

    setup_message_bus(&message_case.bus);

    assert_int_equal(simh_test_capture_stdout(write_scsi_message,
                                              &message_case, &output,
                                              &output_size),
                     0);
    assert_string_equal(output, "");
    assert_int_equal(output_size, 0);
    assert_int_equal(message_case.bus.phase, SCSI_CMD);
    assert_int_equal(message_case.bus.initiator, -1);
    assert_int_equal(message_case.bus.target, -1);

    free(output);
    teardown_message_bus(&message_case.bus);
}

static void test_scsi_message_accepts_save_data_pointers(void **state)
{
    (void)state;

    assert_standard_message_is_accepted_silently(0x02);
}

static void test_scsi_message_accepts_restore_pointers(void **state)
{
    (void)state;

    assert_standard_message_is_accepted_silently(0x03);
}

static void test_scsi_message_accepts_message_reject(void **state)
{
    (void)state;

    assert_standard_message_is_accepted_silently(0x07);
}

static void test_scsi_message_accepts_no_operation(void **state)
{
    (void)state;

    assert_standard_message_is_accepted_silently(0x08);
}

static void test_scsi_message_unknown_still_reports_diagnostic(void **state)
{
    struct scsi_message_case message_case = {
        .message = 0x05,
    };
    char *output = NULL;
    size_t output_size = 0;

    (void)state;

    setup_message_bus(&message_case.bus);

    assert_int_equal(simh_test_capture_stdout(write_scsi_message,
                                              &message_case, &output,
                                              &output_size),
                     0);
    assert_string_equal(output, "SCSI: Unknown Message 05\n");
    assert_int_equal(output_size, strlen(output));
    assert_int_equal(message_case.bus.phase, SCSI_CMD);
    assert_int_equal(message_case.bus.initiator, 6);
    assert_int_equal(message_case.bus.target, 4);

    free(output);
    teardown_message_bus(&message_case.bus);
}

static void test_scsi_message_abort_disconnects(void **state)
{
    (void)state;

    assert_message_disconnects_silently(0x06);
}

static void test_scsi_message_bus_device_reset_disconnects(void **state)
{
    (void)state;

    assert_message_disconnects_silently(0x0c);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_scsi_message_accepts_save_data_pointers),
        cmocka_unit_test(test_scsi_message_accepts_restore_pointers),
        cmocka_unit_test(test_scsi_message_accepts_message_reject),
        cmocka_unit_test(test_scsi_message_accepts_no_operation),
        cmocka_unit_test(test_scsi_message_unknown_still_reports_diagnostic),
        cmocka_unit_test(test_scsi_message_abort_disconnects),
        cmocka_unit_test(test_scsi_message_bus_device_reset_disconnects),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
