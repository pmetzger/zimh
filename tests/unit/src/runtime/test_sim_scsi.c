#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "sim_scsi.h"
#include "test_support.h"

#define TEST_SCSI_STATUS_GOOD 0
#define TEST_SCSI_STATUS_CHECK 2
#define TEST_SCSI_SENSE_ILLEGAL_REQUEST 5
#define TEST_SCSI_ASC_INVALID_FIELD_IN_CDB 0x24

struct scsi_message_case {
    uint8 message;
    SCSI_BUS bus;
};

struct scsi_cdrom_case {
    uint8 command[10];
    uint32 command_len;
    SCSI_BUS bus;
    DEVICE device;
    UNIT unit;
    SCSI_DEV scsi_device;
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

static void setup_cdrom_bus(struct scsi_cdrom_case *cdrom_case)
{
    memset(cdrom_case, 0, sizeof(*cdrom_case));

    simh_test_init_device_unit(&cdrom_case->device, &cdrom_case->unit,
                               "SCSI", "SCSI4", DEV_SECTORS,
                               UNIT_ATTABLE | UNIT_ATT, 16, 1);

    cdrom_case->scsi_device.devtype = SCSI_CDROM;
    cdrom_case->scsi_device.pqual = 0;
    cdrom_case->scsi_device.scsiver = 2;
    cdrom_case->scsi_device.removeable = TRUE;
    cdrom_case->scsi_device.block_size = 2048;
    cdrom_case->scsi_device.manufacturer = "SIMH";
    cdrom_case->scsi_device.product = "CD-ROM";
    cdrom_case->scsi_device.rev = "0001";

    cdrom_case->unit.up7 = &cdrom_case->scsi_device;
    cdrom_case->unit.capac = 0x12345;

    cdrom_case->bus.dptr = &cdrom_case->device;
    assert_int_equal(scsi_init(&cdrom_case->bus, 4096), SCPE_OK);
    scsi_reset(&cdrom_case->bus);
    scsi_add_unit(&cdrom_case->bus, 4, &cdrom_case->unit);

    cdrom_case->bus.initiator = 6;
    cdrom_case->bus.target = 4;
    cdrom_case->bus.phase = SCSI_CMD;
    cdrom_case->bus.req = TRUE;
}

static void teardown_cdrom_bus(struct scsi_cdrom_case *cdrom_case)
{
    free(cdrom_case->bus.buf);
}

static void write_scsi_message(void *context)
{
    struct scsi_message_case *message_case = context;
    uint8 message = message_case->message;

    assert_int_equal(scsi_write(&message_case->bus, &message, 1), 1);
}

static void write_scsi_cdrom_command(void *context)
{
    struct scsi_cdrom_case *cdrom_case = context;

    assert_int_equal(scsi_write(&cdrom_case->bus, cdrom_case->command,
                                cdrom_case->command_len),
                     cdrom_case->command_len);
}

static void assert_cdrom_command_writes_silently(
    struct scsi_cdrom_case *cdrom_case)
{
    char *output = NULL;
    size_t output_size = 0;

    assert_int_equal(simh_test_capture_stdout(write_scsi_cdrom_command,
                                              cdrom_case, &output,
                                              &output_size),
                     0);
    assert_string_equal(output, "");
    assert_int_equal(output_size, 0);

    free(output);
}

static void assert_scsi_data_in(const struct scsi_cdrom_case *cdrom_case,
                                const uint8 *expected, size_t expected_size)
{
    uint8 data[64];

    assert_true(expected_size <= sizeof(data));
    assert_int_equal(cdrom_case->bus.phase, SCSI_DATI);
    assert_int_equal(scsi_read((SCSI_BUS *)&cdrom_case->bus, data,
                               (uint32)expected_size),
                     expected_size);
    assert_memory_equal(data, expected, expected_size);
}

static void assert_scsi_good_status(struct scsi_cdrom_case *cdrom_case)
{
    uint8 status;

    assert_int_equal(cdrom_case->bus.phase, SCSI_STS);
    assert_int_equal(scsi_read(&cdrom_case->bus, &status, 1), 1);
    assert_int_equal(status, TEST_SCSI_STATUS_GOOD);
}

static void assert_scsi_check_status(struct scsi_cdrom_case *cdrom_case,
                                     uint32 sense_key, uint32 sense_code)
{
    uint8 status;

    assert_int_equal(cdrom_case->bus.phase, SCSI_STS);
    assert_int_equal(scsi_read(&cdrom_case->bus, &status, 1), 1);
    assert_int_equal(status, TEST_SCSI_STATUS_CHECK);
    assert_int_equal(cdrom_case->bus.sense_key, sense_key);
    assert_int_equal(cdrom_case->bus.sense_code, sense_code);
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
    assert_int_equal(message_case.bus.phase, SCSI_DATO);
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

static void test_scsi_release_clears_bus_signals_not_sense(void **state)
{
    SCSI_BUS bus;

    (void)state;

    setup_message_bus(&bus);
    bus.atn = TRUE;
    bus.sense_key = 0x05;
    bus.sense_code = 0x24;
    bus.sense_qual = 0x01;
    bus.sense_info = 0x12345678;

    scsi_release(&bus);

    assert_int_equal(bus.phase, SCSI_DATO);
    assert_int_equal(bus.initiator, -1);
    assert_int_equal(bus.target, -1);
    assert_false(bus.atn);
    assert_false(bus.req);
    assert_int_equal(bus.sense_key, 0x05);
    assert_int_equal(bus.sense_code, 0x24);
    assert_int_equal(bus.sense_qual, 0x01);
    assert_int_equal(bus.sense_info, 0x12345678);

    teardown_message_bus(&bus);
}

static void test_scsi_release_cleans_already_free_bus(void **state)
{
    SCSI_BUS bus;

    (void)state;

    setup_message_bus(&bus);
    bus.initiator = -1;
    bus.target = -1;
    bus.phase = SCSI_MSGI;
    bus.atn = TRUE;
    bus.req = TRUE;
    bus.buf_b = 2;
    bus.buf_t = 4;

    scsi_release(&bus);

    assert_int_equal(bus.phase, SCSI_DATO);
    assert_int_equal(bus.initiator, -1);
    assert_int_equal(bus.target, -1);
    assert_false(bus.atn);
    assert_false(bus.req);
    assert_int_equal(bus.buf_b, 0);
    assert_int_equal(bus.buf_t, 0);

    teardown_message_bus(&bus);
}

static void test_scsi_reset_clears_bus_signals_and_sense(void **state)
{
    SCSI_BUS bus;

    (void)state;

    setup_message_bus(&bus);
    bus.atn = TRUE;
    bus.sense_key = 0x05;
    bus.sense_code = 0x24;
    bus.sense_qual = 0x01;
    bus.sense_info = 0x12345678;

    scsi_reset(&bus);

    assert_int_equal(bus.phase, SCSI_DATO);
    assert_int_equal(bus.initiator, -1);
    assert_int_equal(bus.target, -1);
    assert_false(bus.atn);
    assert_false(bus.req);
    assert_int_equal(bus.sense_key, 0);
    assert_int_equal(bus.sense_code, 0);
    assert_int_equal(bus.sense_qual, 0);
    assert_int_equal(bus.sense_info, 0);

    teardown_message_bus(&bus);
}

static void test_cdrom_synchronize_cache_returns_good_status(void **state)
{
    struct scsi_cdrom_case cdrom_case;

    (void)state;

    setup_cdrom_bus(&cdrom_case);
    cdrom_case.command[0] = 0x35;
    cdrom_case.command_len = 10;

    assert_cdrom_command_writes_silently(&cdrom_case);
    assert_scsi_good_status(&cdrom_case);

    teardown_cdrom_bus(&cdrom_case);
}

static void test_cdrom_read_toc_returns_single_data_track(void **state)
{
    struct scsi_cdrom_case cdrom_case;
    const uint8 expected[] = {
        0x00, 0x12,             /* TOC data length */
        0x01, 0x01,             /* first and last track number */
        0x00, 0x14, 0x01, 0x00, /* track 1 data descriptor */
        0x00, 0x00, 0x00, 0x00, /* track 1 starts at LBA 0 */
        0x00, 0x14, 0xaa, 0x00, /* lead-out data descriptor */
        0x00, 0x01, 0x23, 0x45, /* lead-out starts after last block */
    };

    (void)state;

    setup_cdrom_bus(&cdrom_case);
    cdrom_case.command[0] = 0x43;
    cdrom_case.command[7] = 0x00;
    cdrom_case.command[8] = sizeof(expected);
    cdrom_case.command_len = 10;

    assert_cdrom_command_writes_silently(&cdrom_case);
    assert_scsi_data_in(&cdrom_case, expected, sizeof(expected));
    assert_scsi_good_status(&cdrom_case);

    teardown_cdrom_bus(&cdrom_case);
}

static void test_cdrom_read_toc_obeys_allocation_length(void **state)
{
    struct scsi_cdrom_case cdrom_case;
    const uint8 expected[] = {0x00, 0x12, 0x01, 0x01, 0x00, 0x14};

    (void)state;

    setup_cdrom_bus(&cdrom_case);
    cdrom_case.command[0] = 0x43;
    cdrom_case.command[7] = 0x00;
    cdrom_case.command[8] = sizeof(expected);
    cdrom_case.command_len = 10;

    assert_cdrom_command_writes_silently(&cdrom_case);
    assert_scsi_data_in(&cdrom_case, expected, sizeof(expected));
    assert_scsi_good_status(&cdrom_case);

    teardown_cdrom_bus(&cdrom_case);
}

static void test_cdrom_read_toc_format_one_returns_session_info(void **state)
{
    struct scsi_cdrom_case cdrom_case;
    const uint8 expected[] = {
        0x00, 0x0a,             /* TOC data length */
        0x01, 0x01,             /* first and last complete session */
        0x00, 0x14, 0x01, 0x00, /* first track in last session */
        0x00, 0x00, 0x00, 0x00, /* last session starts at LBA 0 */
    };

    (void)state;

    setup_cdrom_bus(&cdrom_case);
    cdrom_case.command[0] = 0x43;
    cdrom_case.command[2] = 0x01;
    cdrom_case.command[7] = 0x00;
    cdrom_case.command[8] = sizeof(expected);
    cdrom_case.command_len = 10;

    assert_cdrom_command_writes_silently(&cdrom_case);
    assert_scsi_data_in(&cdrom_case, expected, sizeof(expected));
    assert_scsi_good_status(&cdrom_case);

    teardown_cdrom_bus(&cdrom_case);
}

static void test_cdrom_read_toc_rejects_unsupported_format(void **state)
{
    struct scsi_cdrom_case cdrom_case;

    (void)state;

    setup_cdrom_bus(&cdrom_case);
    cdrom_case.command[0] = 0x43;
    cdrom_case.command[2] = 0x04;
    cdrom_case.command[7] = 0x00;
    cdrom_case.command[8] = 12;
    cdrom_case.command_len = 10;

    assert_cdrom_command_writes_silently(&cdrom_case);
    assert_scsi_check_status(&cdrom_case, TEST_SCSI_SENSE_ILLEGAL_REQUEST,
                             TEST_SCSI_ASC_INVALID_FIELD_IN_CDB);

    teardown_cdrom_bus(&cdrom_case);
}

static void test_cdrom_read_disc_information_returns_complete_cdrom(
    void **state)
{
    struct scsi_cdrom_case cdrom_case;
    const uint8 expected[] = {
        0x00, 0x20, /* disc information length */
        0x0e,       /* complete disc, complete last session, not erasable */
        0x01,       /* first track on disc */
        0x01,       /* number of sessions */
        0x01,       /* first track in last session */
        0x01,       /* last track in last session */
        0x00,       /* unrestricted-use flags not meaningful for CD-ROM */
        0x00,       /* CD-DA or CD-ROM */
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, /* no disc identification */
        0xff, 0xff, 0xff, 0x00, /* complete disc lead-in start time */
        0xff, 0xff, 0xff, 0x00, /* complete disc lead-out start limit */
        0x00, 0x00, 0x00, 0x00, /* no disc bar code */
        0x00, 0x00, 0x00, 0x00,
        0x00,                   /* reserved */
        0x00,                   /* no OPC entries */
    };

    (void)state;

    setup_cdrom_bus(&cdrom_case);
    cdrom_case.command[0] = 0x51;
    cdrom_case.command[7] = 0x00;
    cdrom_case.command[8] = sizeof(expected);
    cdrom_case.command_len = 10;

    assert_cdrom_command_writes_silently(&cdrom_case);
    assert_scsi_data_in(&cdrom_case, expected, sizeof(expected));
    assert_scsi_good_status(&cdrom_case);

    teardown_cdrom_bus(&cdrom_case);
}

static void test_cdrom_read_disc_information_obeys_allocation_length(
    void **state)
{
    struct scsi_cdrom_case cdrom_case;
    const uint8 expected[] = {0x00, 0x20, 0x0e, 0x01, 0x01, 0x01};

    (void)state;

    setup_cdrom_bus(&cdrom_case);
    cdrom_case.command[0] = 0x51;
    cdrom_case.command[7] = 0x00;
    cdrom_case.command[8] = sizeof(expected);
    cdrom_case.command_len = 10;

    assert_cdrom_command_writes_silently(&cdrom_case);
    assert_scsi_data_in(&cdrom_case, expected, sizeof(expected));
    assert_scsi_good_status(&cdrom_case);

    teardown_cdrom_bus(&cdrom_case);
}

static void test_cdrom_read_track_information_returns_complete_data_track(
    void **state)
{
    struct scsi_cdrom_case cdrom_case;
    const uint8 expected[] = {
        0x00, 0x22,                         /* track info length */
        0x01, 0x01,                         /* track and session */
        0x00, 0x04, 0x01, 0x02,             /* data track, LRA valid */
        0x00, 0x00, 0x00, 0x00,             /* track start */
        0x00, 0x00, 0x00, 0x00,             /* next writable address */
        0x00, 0x00, 0x00, 0x00,             /* free blocks */
        0x00, 0x00, 0x00, 0x00,             /* packet size */
        0x00, 0x01, 0x23, 0x45,             /* track size */
        0x00, 0x01, 0x23, 0x44,             /* last recorded block */
        0x00, 0x00, 0x00, 0x00,             /* track/session MSB, reserved */
    };

    (void)state;

    setup_cdrom_bus(&cdrom_case);
    cdrom_case.command[0] = 0x52;
    cdrom_case.command[1] = 0x01;
    cdrom_case.command[5] = 0x01;
    cdrom_case.command[7] = 0x00;
    cdrom_case.command[8] = sizeof(expected);
    cdrom_case.command_len = 10;

    assert_cdrom_command_writes_silently(&cdrom_case);
    assert_scsi_data_in(&cdrom_case, expected, sizeof(expected));
    assert_scsi_good_status(&cdrom_case);

    teardown_cdrom_bus(&cdrom_case);
}

static void test_cdrom_read_track_information_obeys_allocation_length(
    void **state)
{
    struct scsi_cdrom_case cdrom_case;
    const uint8 expected[] = {0x00, 0x22, 0x01, 0x01, 0x00, 0x04};

    (void)state;

    setup_cdrom_bus(&cdrom_case);
    cdrom_case.command[0] = 0x52;
    cdrom_case.command[1] = 0x01;
    cdrom_case.command[5] = 0x01;
    cdrom_case.command[7] = 0x00;
    cdrom_case.command[8] = sizeof(expected);
    cdrom_case.command_len = 10;

    assert_cdrom_command_writes_silently(&cdrom_case);
    assert_scsi_data_in(&cdrom_case, expected, sizeof(expected));
    assert_scsi_good_status(&cdrom_case);

    teardown_cdrom_bus(&cdrom_case);
}

static void test_cdrom_read_track_information_rejects_unsupported_address_type(
    void **state)
{
    struct scsi_cdrom_case cdrom_case;

    (void)state;

    setup_cdrom_bus(&cdrom_case);
    cdrom_case.command[0] = 0x52;
    cdrom_case.command[1] = 0x02;
    cdrom_case.command[5] = 0x01;
    cdrom_case.command[7] = 0x00;
    cdrom_case.command[8] = 36;
    cdrom_case.command_len = 10;

    assert_cdrom_command_writes_silently(&cdrom_case);
    assert_scsi_check_status(&cdrom_case, TEST_SCSI_SENSE_ILLEGAL_REQUEST,
                             TEST_SCSI_ASC_INVALID_FIELD_IN_CDB);

    teardown_cdrom_bus(&cdrom_case);
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
        cmocka_unit_test(test_scsi_release_clears_bus_signals_not_sense),
        cmocka_unit_test(test_scsi_release_cleans_already_free_bus),
        cmocka_unit_test(test_scsi_reset_clears_bus_signals_and_sense),
        cmocka_unit_test(test_cdrom_synchronize_cache_returns_good_status),
        cmocka_unit_test(test_cdrom_read_toc_returns_single_data_track),
        cmocka_unit_test(test_cdrom_read_toc_obeys_allocation_length),
        cmocka_unit_test(test_cdrom_read_toc_format_one_returns_session_info),
        cmocka_unit_test(test_cdrom_read_toc_rejects_unsupported_format),
        cmocka_unit_test(
            test_cdrom_read_disc_information_returns_complete_cdrom),
        cmocka_unit_test(
            test_cdrom_read_disc_information_obeys_allocation_length),
        cmocka_unit_test(
            test_cdrom_read_track_information_returns_complete_data_track),
        cmocka_unit_test(
            test_cdrom_read_track_information_obeys_allocation_length),
        cmocka_unit_test(
            test_cdrom_read_track_information_rejects_unsupported_address_type),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
