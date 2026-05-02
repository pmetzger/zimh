#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "test_cmocka.h"

#include "sim_defs.h"
#include "sim_tape.h"
#include "test_scp_fixture.h"
#include "test_simh_personality.h"
#include "test_support.h"

struct sim_tape_fixture {
    DEVICE device;
    UNIT unit;
    DEVICE *devices[2];
    char temp_dir[1024];
    char original_cwd[1024];
    char tape_path[1024];
};

struct sim_tape_capture {
    struct sim_tape_fixture *fixture;
    t_stat status;
};

struct sim_tape_callback_state {
    UNIT *unit;
    t_stat status;
    unsigned calls;
};

static struct sim_tape_callback_state *active_tape_callback_state;

static int setup_sim_tape_fixture(void **state)
{
    struct sim_tape_fixture *fixture;

    fixture = calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    simh_test_init_device_unit(&fixture->device, &fixture->unit, "TAPE",
                               "TAPE0", DEV_DISABLE, UNIT_ATTABLE, 8, 1);
    fixture->unit.dynflags = MTUF_F_STD << UNIT_V_TAPE_FMT;
    fixture->devices[0] = &fixture->device;
    fixture->devices[1] = NULL;

    assert_int_equal(simh_test_make_temp_dir(fixture->temp_dir,
                                             sizeof(fixture->temp_dir),
                                             "sim-tape"),
                     0);
    assert_non_null(
        getcwd(fixture->original_cwd, sizeof(fixture->original_cwd)));
    assert_int_equal(simh_test_join_path(fixture->tape_path,
                                         sizeof(fixture->tape_path),
                                         fixture->temp_dir, "sample.tap"),
                     0);
    assert_int_equal(
        simh_test_install_devices("zimh-unit-sim-tape", fixture->devices),
        0);
    assert_int_equal(sim_tape_init(), SCPE_OK);

    *state = fixture;
    return 0;
}

static int teardown_sim_tape_fixture(void **state)
{
    struct sim_tape_fixture *fixture = *state;

    if (fixture->unit.flags & UNIT_ATT)
        assert_int_equal(sim_tape_detach(&fixture->unit), SCPE_OK);
    assert_int_equal(chdir(fixture->original_cwd), 0);
    assert_int_equal(simh_test_remove_path(fixture->temp_dir), 0);
    simh_test_reset_simulator_state();
    free(fixture);
    *state = NULL;
    return 0;
}

static void assert_show_output(t_stat (*show_fn)(FILE *, UNIT *, int32,
                                                 const void *),
                               UNIT *uptr, const char *expected)
{
    FILE *stream;
    char *text;
    size_t size;

    stream = tmpfile();
    assert_non_null(stream);

    assert_int_equal(show_fn(stream, uptr, 0, NULL), SCPE_OK);
    assert_int_equal(simh_test_read_stream(stream, &text, &size), 0);
    assert_string_equal(text, expected);

    free(text);
    fclose(stream);
}

static void assert_tape_record_equals(const uint8 *actual, t_mtrlnt actual_len,
                                      const uint8 *expected,
                                      t_mtrlnt expected_len)
{
    assert_int_equal(actual_len, expected_len);
    assert_memory_equal(actual, expected, expected_len);
}

static void assert_string_contains(const char *haystack, const char *needle)
{
    assert_non_null(strstr(haystack, needle));
}

static void record_tape_callback(UNIT *unit, t_stat status)
{
    assert_non_null(active_tape_callback_state);
    active_tape_callback_state->unit = unit;
    active_tape_callback_state->status = status;
    active_tape_callback_state->calls += 1;
}

static void reset_tape_callback_state(struct sim_tape_callback_state *state)
{
    memset(state, 0, sizeof(*state));
}

static void assert_tape_callback(struct sim_tape_callback_state *state,
                                 UNIT *unit, t_stat status)
{
    assert_int_equal(state->calls, 1);
    assert_ptr_equal(state->unit, unit);
    assert_int_equal(state->status, status);
    reset_tape_callback_state(state);
}

static void write_sim_tape_self_test_output(void *context)
{
    struct sim_tape_capture *capture = context;

    assert_int_equal(chdir(capture->fixture->temp_dir), 0);
    capture->status = sim_tape_test(&capture->fixture->device, NULL);
    assert_int_equal(chdir(capture->fixture->original_cwd), 0);
}

/* Verify the BOT/EOT/write-protect predicates track format, position,
   and tape flags. */
static void test_sim_tape_status_predicates_follow_unit_state(void **state)
{
    struct sim_tape_fixture *fixture = *state;

    fixture->unit.pos = 0;
    assert_true(sim_tape_bot(&fixture->unit));

    fixture->unit.pos = sizeof(t_mtrlnt);
    assert_false(sim_tape_bot(&fixture->unit));

    fixture->unit.pos = 0;
    MT_SET_INMRK(&fixture->unit);
    assert_false(sim_tape_bot(&fixture->unit));
    MT_CLR_INMRK(&fixture->unit);

    fixture->unit.capac = 0;
    assert_false(sim_tape_eot(&fixture->unit));

    fixture->unit.capac = 100;
    fixture->unit.pos = 99;
    assert_false(sim_tape_eot(&fixture->unit));
    fixture->unit.pos = 100;
    assert_true(sim_tape_eot(&fixture->unit));

    fixture->unit.flags = UNIT_ATTABLE;
    fixture->unit.dynflags = MTUF_F_STD << UNIT_V_TAPE_FMT;
    assert_false(sim_tape_wrp(&fixture->unit));

    fixture->unit.flags |= MTUF_WRP;
    assert_true(sim_tape_wrp(&fixture->unit));

    fixture->unit.flags = UNIT_ATTABLE;
    fixture->unit.dynflags = MTUF_F_TPC << UNIT_V_TAPE_FMT;
    assert_true(sim_tape_wrp(&fixture->unit));
}

/* Verify tape format changes update flags correctly and the show helper
   reports the selected format. */
static void test_sim_tape_set_and_show_format(void **state)
{
    struct sim_tape_fixture *fixture = *state;

    assert_int_equal(sim_tape_set_fmt(&fixture->unit, 0, "TPC", NULL), SCPE_OK);
    assert_int_equal(MT_GET_FMT(&fixture->unit), MTUF_F_TPC);
    assert_true((fixture->unit.flags & UNIT_RO) != 0);
    assert_show_output(sim_tape_show_fmt, &fixture->unit, "TPC format");

    assert_true(sim_tape_set_fmt(&fixture->unit, 0, "UNKNOWN", NULL) !=
                SCPE_OK);
}

/* Verify ANSI memory-tape subformats select both the generic format and
   the ANSI subtype, and reject unknown variants. */
static void test_sim_tape_set_fmt_supports_ansi_variants(void **state)
{
    struct sim_tape_fixture *fixture = *state;

    assert_int_equal(sim_tape_set_fmt(&fixture->unit, 0, "ANSI-RSTS", NULL),
                     SCPE_OK);
    assert_int_equal(MT_GET_FMT(&fixture->unit), MTUF_F_ANSI);
    assert_show_output(sim_tape_show_fmt, &fixture->unit, "ANSI-RSTS format");

    assert_int_equal(sim_tape_set_fmt(&fixture->unit, 0, "ANSI-VMS", NULL),
                     SCPE_OK);
    assert_int_equal(MT_GET_FMT(&fixture->unit), MTUF_F_ANSI);
    assert_show_output(sim_tape_show_fmt, &fixture->unit, "ANSI-VMS format");

    assert_true(sim_tape_set_fmt(&fixture->unit, 0, "ANSI-BOGUS", NULL) !=
                SCPE_OK);
}

/* Verify capacity parsing stores megabytes and the show helper formats
   the stored size. */
static void test_sim_tape_set_and_show_capacity(void **state)
{
    struct sim_tape_fixture *fixture = *state;

    assert_int_equal(sim_tape_set_capac(&fixture->unit, 0, "25", NULL),
                     SCPE_OK);
    assert_int_equal(fixture->unit.capac, ((t_addr)25 * 1000000));
    assert_show_output(sim_tape_show_capac, &fixture->unit, "capacity=25MB");

    fixture->unit.flags |= UNIT_ATT;
    assert_int_equal(sim_tape_set_capac(&fixture->unit, 0, "5", NULL),
                     SCPE_ALATT);
}

/* Verify density parsing works for both direct and validated-string
   entry paths, and that supported-density formatting is stable. */
static void test_sim_tape_density_helpers_validate_and_render(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    int32 valid_mask = MT_800_VALID | MT_1600_VALID | MT_6250_VALID;
    char density_list[64];

    assert_int_equal(
        sim_tape_set_dens(&fixture->unit, MT_DENS_1600, NULL, NULL), SCPE_OK);
    assert_int_equal(MT_DENS(fixture->unit.dynflags), MT_DENS_1600);
    assert_show_output(sim_tape_show_dens, &fixture->unit, "density=1600 bpi");

    assert_int_equal(sim_tape_set_dens(&fixture->unit, 0, "6250", &valid_mask),
                     SCPE_OK);
    assert_int_equal(MT_DENS(fixture->unit.dynflags), MT_DENS_6250);
    assert_show_output(sim_tape_show_dens, &fixture->unit, "density=6250 bpi");

    assert_int_equal(sim_tape_set_dens(&fixture->unit, 0, NULL, &valid_mask),
                     SCPE_MISVAL);
    assert_int_equal(sim_tape_set_dens(&fixture->unit, 0, "556", &valid_mask),
                     SCPE_ARG);

    assert_int_equal(sim_tape_density_supported(
                         density_list, sizeof(density_list), MT_1600_VALID),
                     SCPE_OK);
    assert_string_equal(density_list, "1600");

    assert_int_equal(sim_tape_density_supported(
                         density_list, sizeof(density_list), valid_mask),
                     SCPE_OK);
    assert_string_equal(density_list, "{800|1600|6250}");
}

/* Verify textual error translation covers named tape errors and generic
   numeric fallback. */
static void
test_sim_tape_error_text_covers_named_and_generic_errors(void **state)
{
    (void)state;

    assert_string_equal(sim_tape_error_text(MTSE_OK), "no error");
    assert_string_equal(sim_tape_error_text(MTSE_WRP), "write protected");
    assert_string_equal(sim_tape_error_text(MTSE_RUNAWAY), "tape runaway");
    assert_string_equal(sim_tape_error_text(MTSE_MAX_ERR + 5), "Error 16");
}

/* Verify attach-help text mentions the major switches, ANSI formats,
   and example commands. */
static void test_sim_tape_attach_help_describes_supported_usage(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    FILE *stream;
    char *text;
    size_t size;

    stream = tmpfile();
    assert_non_null(stream);

    assert_int_equal(
        sim_tape_attach_help(stream, &fixture->device, &fixture->unit, 0, NULL),
        SCPE_OK);
    assert_int_equal(simh_test_read_stream(stream, &text, &size), 0);
    assert_string_contains(text, "TAPE Tape Attach Help");
    assert_string_contains(text, "-F          Open the indicated tape");
    assert_string_contains(text, "ANSI-VMS, ANSI-RT11");
    assert_string_contains(text, "sim> ATTACH TAPE -F DOS11");

    free(text);
    fclose(stream);
}

/* Verify the built-in sim_tape self-test succeeds in a temporary
   workspace and reports the API test run. */
static void test_sim_tape_self_test_succeeds(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    struct sim_tape_capture capture = {fixture, SCPE_IERR};
    char *text;
    size_t size;
    char generated_path[1024];

    assert_int_equal(simh_test_capture_stdout(write_sim_tape_self_test_output,
                                              &capture, &text, &size),
                     0);
    assert_int_equal(capture.status, SCPE_OK);
    assert_int_equal(simh_test_join_path(generated_path, sizeof(generated_path),
                                         fixture->temp_dir, "TapeTestFile1"),
                     0);
    assert_true(access(generated_path, F_OK) != 0);
    free(text);
}

/* Verify the built-in sim_tape self-test rejects attached units rather
   than mutating live tape state. */
static void test_sim_tape_self_test_rejects_attached_units(void **state)
{
    struct sim_tape_fixture *fixture = *state;

    assert_int_equal(sim_tape_attach(&fixture->unit, fixture->tape_path),
                     SCPE_OK);
    assert_int_equal(SCPE_BARE_STATUS(sim_tape_test(&fixture->device, NULL)),
                     SCPE_ALATT);
}

/* Verify a minimal set of synchronous *_a wrappers returns the same
   status as the base operations and invokes the callback once. */
static void test_sim_tape_callback_wrappers_report_sync_status(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    struct sim_tape_callback_state callback_state;
    uint8 record[] = {0x61, 0x62, 0x63};
    uint8 read_buffer[16] = {0};
    t_mtrlnt record_length;

    active_tape_callback_state = &callback_state;
    reset_tape_callback_state(&callback_state);

    assert_int_equal(sim_tape_attach(&fixture->unit, fixture->tape_path),
                     SCPE_OK);
    assert_int_equal(
        sim_tape_set_dens(&fixture->unit, MT_DENS_1600, NULL, NULL), SCPE_OK);

    assert_int_equal(sim_tape_wrgap_a(&fixture->unit, 0, record_tape_callback),
                     MTSE_OK);
    assert_tape_callback(&callback_state, &fixture->unit, MTSE_OK);

    assert_int_equal(sim_tape_wrrecf_a(&fixture->unit, record, sizeof(record),
                                       record_tape_callback),
                     MTSE_OK);
    assert_tape_callback(&callback_state, &fixture->unit, MTSE_OK);

    assert_int_equal(sim_tape_rewind_a(&fixture->unit, record_tape_callback),
                     MTSE_OK);
    assert_tape_callback(&callback_state, &fixture->unit, MTSE_OK);

    assert_int_equal(sim_tape_rdrecf_a(&fixture->unit, read_buffer,
                                       &record_length, sizeof(read_buffer),
                                       record_tape_callback),
                     MTSE_OK);
    assert_tape_callback(&callback_state, &fixture->unit, MTSE_OK);
    assert_tape_record_equals(read_buffer, record_length, record,
                              sizeof(record));

    active_tape_callback_state = NULL;
}

/* Verify the synchronous callback wrappers return the same status as the
   base APIs and report it via the supplied callback. */
/* Verify a standard tape image can be attached, written, rewound, and
   read back including tape marks and end-of-medium status. */
static void test_sim_tape_standard_image_round_trip(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    uint8 first_record[] = {0x01, 0x02, 0x03};
    uint8 second_record[] = {0xAA, 0xBB};
    uint8 read_buffer[16] = {0};
    t_mtrlnt record_length;

    assert_int_equal(sim_tape_attach(&fixture->unit, fixture->tape_path),
                     SCPE_OK);
    assert_true((fixture->unit.flags & UNIT_ATT) != 0);

    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, first_record, sizeof(first_record)),
        MTSE_OK);
    assert_int_equal(sim_tape_wrtmk(&fixture->unit), MTSE_OK);
    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, second_record, sizeof(second_record)),
        MTSE_OK);
    assert_true(fixture->unit.tape_eom > 0);

    assert_int_equal(sim_tape_rewind(&fixture->unit), MTSE_OK);
    memset(read_buffer, 0, sizeof(read_buffer));
    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_OK);
    assert_tape_record_equals(read_buffer, record_length, first_record,
                              sizeof(first_record));

    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_TMK);
    assert_int_equal(record_length, 0);

    memset(read_buffer, 0, sizeof(read_buffer));
    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_OK);
    assert_tape_record_equals(read_buffer, record_length, second_record,
                              sizeof(second_record));

    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_EOM);
    assert_int_equal(record_length, 0);
}

/* Verify forward and reverse spacing move across records and file
   boundaries as expected on a standard tape image. */
static void test_sim_tape_spacing_and_reverse_reads_work(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    uint8 first_record[] = {0x10, 0x11, 0x12, 0x13};
    uint8 second_record[] = {0x20, 0x21};
    uint8 read_buffer[16] = {0};
    t_mtrlnt record_length;
    uint32 skipped;

    assert_int_equal(sim_tape_attach(&fixture->unit, fixture->tape_path),
                     SCPE_OK);
    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, first_record, sizeof(first_record)),
        MTSE_OK);
    assert_int_equal(sim_tape_wrtmk(&fixture->unit), MTSE_OK);
    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, second_record, sizeof(second_record)),
        MTSE_OK);
    assert_int_equal(sim_tape_rewind(&fixture->unit), MTSE_OK);

    assert_int_equal(sim_tape_sprecf(&fixture->unit, &record_length), MTSE_OK);
    assert_int_equal(record_length, sizeof(first_record));

    assert_int_equal(sim_tape_spfilef(&fixture->unit, 1, &skipped), MTSE_OK);
    assert_int_equal(skipped, 1);

    memset(read_buffer, 0, sizeof(read_buffer));
    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_OK);
    assert_tape_record_equals(read_buffer, record_length, second_record,
                              sizeof(second_record));

    memset(read_buffer, 0, sizeof(read_buffer));
    assert_int_equal(sim_tape_rdrecr(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_OK);
    assert_tape_record_equals(read_buffer, record_length, second_record,
                              sizeof(second_record));

    assert_int_equal(sim_tape_rdrecr(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_TMK);
    assert_int_equal(record_length, 0);

    memset(read_buffer, 0, sizeof(read_buffer));
    assert_int_equal(sim_tape_rdrecr(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_OK);
    assert_tape_record_equals(read_buffer, record_length, first_record,
                              sizeof(first_record));
}

/* Verify operational error paths on attached tapes: short read buffers,
   write protection, and detach state cleanup. */
static void test_sim_tape_operational_error_paths(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    uint8 record[] = {0x33, 0x44, 0x55, 0x66};
    uint8 short_buffer[2] = {0};
    t_mtrlnt record_length;

    assert_int_equal(sim_tape_attach(&fixture->unit, fixture->tape_path),
                     SCPE_OK);
    assert_int_equal(sim_tape_wrrecf(&fixture->unit, record, sizeof(record)),
                     MTSE_OK);
    assert_int_equal(sim_tape_rewind(&fixture->unit), MTSE_OK);

    assert_int_equal(sim_tape_rdrecf(&fixture->unit, short_buffer,
                                     &record_length, sizeof(short_buffer)),
                     MTSE_INVRL);
    assert_true(MT_TST_PNU(&fixture->unit));

    MT_CLR_PNU(&fixture->unit);
    fixture->unit.flags |= MTUF_WRP;
    assert_int_equal(sim_tape_wrrecf(&fixture->unit, record, sizeof(record)),
                     MTSE_WRP);
    fixture->unit.flags &= ~MTUF_WRP;

    assert_int_equal(sim_tape_detach(&fixture->unit), SCPE_OK);
    assert_false((fixture->unit.flags & UNIT_ATT) != 0);
    assert_int_equal(fixture->unit.pos, 0);
    assert_int_equal(fixture->unit.tape_eom, 0);
    assert_null(fixture->unit.fileref);
    assert_null(fixture->unit.io_flush);
}

/* Verify generic positioning can count objects and position by
   file/record tuples after an optional rewind. */
static void test_sim_tape_position_tracks_objects_and_files(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    uint8 first_record[] = {0x01, 0x03, 0x05};
    uint8 second_record[] = {0x02, 0x04};
    uint8 third_record[] = {0x06};
    uint8 read_buffer[16] = {0};
    t_mtrlnt record_length;
    uint32 records_skipped;
    uint32 files_skipped;
    uint32 objects_skipped;

    assert_int_equal(sim_tape_attach(&fixture->unit, fixture->tape_path),
                     SCPE_OK);
    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, first_record, sizeof(first_record)),
        MTSE_OK);
    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, second_record, sizeof(second_record)),
        MTSE_OK);
    assert_int_equal(sim_tape_wrtmk(&fixture->unit), MTSE_OK);
    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, third_record, sizeof(third_record)),
        MTSE_OK);

    assert_int_equal(sim_tape_position(
                         &fixture->unit, MTPOS_M_REW | MTPOS_M_OBJ, 3,
                         &records_skipped, 0, &files_skipped, &objects_skipped),
                     MTSE_OK);
    assert_int_equal(records_skipped, 0);
    assert_int_equal(files_skipped, 0);
    assert_int_equal(objects_skipped, 3);

    memset(read_buffer, 0, sizeof(read_buffer));
    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_OK);
    assert_tape_record_equals(read_buffer, record_length, third_record,
                              sizeof(third_record));

    assert_int_equal(sim_tape_position(&fixture->unit, MTPOS_M_REW, 1,
                                       &records_skipped, 1, &files_skipped,
                                       &objects_skipped),
                     MTSE_OK);
    assert_int_equal(records_skipped, 1);
    assert_int_equal(files_skipped, 1);
    assert_int_equal(objects_skipped, 4);

    memset(read_buffer, 0, sizeof(read_buffer));
    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_EOM);
    assert_int_equal(record_length, 0);
}

/* Verify direct file-spacing reports both file and record counts and
   detects logical end of tape on a double tape mark. */
static void test_sim_tape_spfilebyrecf_reports_counts_and_leot(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    uint8 first_record[] = {0x0A, 0x0B};
    uint8 read_buffer[16] = {0};
    t_mtrlnt record_length;
    uint32 files_skipped;
    uint32 records_skipped;

    assert_int_equal(sim_tape_attach(&fixture->unit, fixture->tape_path),
                     SCPE_OK);
    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, first_record, sizeof(first_record)),
        MTSE_OK);
    assert_int_equal(sim_tape_wrtmk(&fixture->unit), MTSE_OK);
    assert_int_equal(sim_tape_wrtmk(&fixture->unit), MTSE_OK);
    assert_int_equal(sim_tape_rewind(&fixture->unit), MTSE_OK);

    assert_int_equal(sim_tape_spfilebyrecf(&fixture->unit, 1, &files_skipped,
                                           &records_skipped, FALSE),
                     MTSE_OK);
    assert_int_equal(files_skipped, 1);
    assert_int_equal(records_skipped, 1);

    memset(read_buffer, 0, sizeof(read_buffer));
    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_TMK);
    assert_int_equal(record_length, 0);

    assert_int_equal(sim_tape_rewind(&fixture->unit), MTSE_OK);
    assert_int_equal(sim_tape_spfilebyrecf(&fixture->unit, 2, &files_skipped,
                                           &records_skipped, TRUE),
                     MTSE_LEOT);
    assert_int_equal(files_skipped, 1);
    assert_int_equal(records_skipped, 1);
}

/* Verify reverse file-spacing reports both file and record counts and
   leaves the unit positioned at the preceding file boundary. */
static void test_sim_tape_spfilebyrecr_reports_counts(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    uint8 first_record[] = {0x31, 0x32};
    uint8 second_record[] = {0x41, 0x42, 0x43};
    uint8 read_buffer[16] = {0};
    t_mtrlnt record_length;
    uint32 files_skipped;
    uint32 records_skipped;

    assert_int_equal(sim_tape_attach(&fixture->unit, fixture->tape_path),
                     SCPE_OK);
    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, first_record, sizeof(first_record)),
        MTSE_OK);
    assert_int_equal(sim_tape_wrtmk(&fixture->unit), MTSE_OK);
    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, second_record, sizeof(second_record)),
        MTSE_OK);

    assert_int_equal(sim_tape_spfilebyrecr(&fixture->unit, 1, &files_skipped,
                                           &records_skipped),
                     MTSE_OK);
    assert_int_equal(files_skipped, 1);
    assert_int_equal(records_skipped, 1);

    memset(read_buffer, 0, sizeof(read_buffer));
    assert_int_equal(sim_tape_rdrecr(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_OK);
    assert_tape_record_equals(read_buffer, record_length, first_record,
                              sizeof(first_record));
}

/* Verify write-EOM-and-rewind leaves the unit rewound and makes the
   newly written EOM visible after the existing data. */
static void test_sim_tape_wreomrw_writes_eom_and_rewinds(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    uint8 record[] = {0x11, 0x22, 0x33};
    uint8 read_buffer[16] = {0};
    t_mtrlnt record_length;

    assert_int_equal(sim_tape_attach(&fixture->unit, fixture->tape_path),
                     SCPE_OK);
    assert_int_equal(sim_tape_wrrecf(&fixture->unit, record, sizeof(record)),
                     MTSE_OK);
    MT_SET_PNU(&fixture->unit);

    assert_int_equal(sim_tape_wreomrw(&fixture->unit), MTSE_OK);
    assert_true(sim_tape_bot(&fixture->unit));
    assert_false(MT_TST_PNU(&fixture->unit));

    memset(read_buffer, 0, sizeof(read_buffer));
    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_OK);
    assert_tape_record_equals(read_buffer, record_length, record,
                              sizeof(record));

    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_EOM);
    assert_int_equal(record_length, 0);
}

/* Verify reset clears position-not-updated state without detaching or
   rewinding an attached tape. */
static void test_sim_tape_reset_clears_pnu_without_detaching(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    uint8 record[] = {0x09, 0x08, 0x07};
    t_addr saved_pos;

    assert_int_equal(sim_tape_attach(&fixture->unit, fixture->tape_path),
                     SCPE_OK);
    assert_int_equal(sim_tape_wrrecf(&fixture->unit, record, sizeof(record)),
                     MTSE_OK);
    saved_pos = fixture->unit.pos;
    MT_SET_PNU(&fixture->unit);

    assert_int_equal(sim_tape_reset(&fixture->unit), SCPE_OK);
    assert_false(MT_TST_PNU(&fixture->unit));
    assert_true((fixture->unit.flags & UNIT_ATT) != 0);
    assert_int_equal(fixture->unit.pos, saved_pos);
}

/* Verify erase-gap and erase-record operations modify standard tapes in
   the expected direction-sensitive ways. */
static void test_sim_tape_gap_and_erase_operations_modify_records(void **state)
{
    struct sim_tape_fixture *fixture = *state;
    uint8 first_record[] = {0x21, 0x22, 0x23};
    uint8 second_record[] = {0x51, 0x52};
    uint8 read_buffer[16] = {0};
    t_mtrlnt record_length;

    assert_int_equal(sim_tape_attach(&fixture->unit, fixture->tape_path),
                     SCPE_OK);
    assert_int_equal(sim_tape_wrgap(&fixture->unit, 1), MTSE_IOERR);
    assert_int_equal(
        sim_tape_set_dens(&fixture->unit, MT_DENS_1600, NULL, NULL), SCPE_OK);
    assert_int_equal(sim_tape_wrgap(&fixture->unit, 0), MTSE_OK);

    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, first_record, sizeof(first_record)),
        MTSE_OK);
    assert_int_equal(
        sim_tape_wrrecf(&fixture->unit, second_record, sizeof(second_record)),
        MTSE_OK);
    assert_int_equal(sim_tape_rewind(&fixture->unit), MTSE_OK);

    assert_int_equal(sim_tape_errecf(&fixture->unit, sizeof(first_record)),
                     MTSE_OK);
    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_OK);
    assert_tape_record_equals(read_buffer, record_length, second_record,
                              sizeof(second_record));

    assert_int_equal(sim_tape_errecr(&fixture->unit, sizeof(second_record)),
                     MTSE_OK);
    assert_int_equal(sim_tape_rewind(&fixture->unit), MTSE_OK);
    assert_int_equal(sim_tape_rdrecf(&fixture->unit, read_buffer,
                                     &record_length, sizeof(read_buffer)),
                     MTSE_RUNAWAY);
    assert_int_equal(record_length, 0);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_sim_tape_status_predicates_follow_unit_state,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(test_sim_tape_set_and_show_format,
                                        setup_sim_tape_fixture,
                                        teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_set_fmt_supports_ansi_variants,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(test_sim_tape_set_and_show_capacity,
                                        setup_sim_tape_fixture,
                                        teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_density_helpers_validate_and_render,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_attach_help_describes_supported_usage,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(test_sim_tape_self_test_succeeds,
                                        setup_sim_tape_fixture,
                                        teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_self_test_rejects_attached_units,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_callback_wrappers_report_sync_status,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(test_sim_tape_standard_image_round_trip,
                                        setup_sim_tape_fixture,
                                        teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_spacing_and_reverse_reads_work,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_spfilebyrecf_reports_counts_and_leot,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_spfilebyrecr_reports_counts, setup_sim_tape_fixture,
            teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_position_tracks_objects_and_files,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_wreomrw_writes_eom_and_rewinds,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_reset_clears_pnu_without_detaching,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(
            test_sim_tape_gap_and_erase_operations_modify_records,
            setup_sim_tape_fixture, teardown_sim_tape_fixture),
        cmocka_unit_test_setup_teardown(test_sim_tape_operational_error_paths,
                                        setup_sim_tape_fixture,
                                        teardown_sim_tape_fixture),
        cmocka_unit_test(
            test_sim_tape_error_text_covers_named_and_generic_errors),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
