#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cmocka.h>

#include "sim_defs.h"
#include "sim_tmxr.h"

struct sim_tmxr_fixture {
    DEVICE device;
    UNIT unit;
    TMXR mux;
    int32 lnorder[4];
    TMLN lines[4];
};

static void ensure_line_tx_buffer(TMLN *line, int32 size)
{
    free(line->txb);
    line->txb = calloc((size_t)size, sizeof(*line->txb));
    assert_non_null(line->txb);
    line->txbsz = size;
    line->txbpr = 0;
    line->txbpi = 0;
    line->txcnt = 0;
    line->txdrp = 0;
    line->txstall = 0;
}

static char *copy_line_tx_buffer(const TMLN *line)
{
    int32 used = tmxr_tqln(line);
    char *copy = calloc((size_t)used + 1, sizeof(*copy));
    int32 i;

    assert_non_null(copy);
    for (i = 0; i < used; i++)
        copy[i] = line->txb[(line->txbpr + i) % line->txbsz];
    copy[used] = '\0';
    return copy;
}

static char *make_temp_log_path(void)
{
    char *path = strdup("/tmp/zimh-tmxr-log-XXXXXX");
    int fd;

    assert_non_null(path);
    fd = mkstemp(path);
    assert_true(fd >= 0);
    close(fd);
    unlink(path);
    return path;
}

static int setup_sim_tmxr_fixture(void **state)
{
    struct sim_tmxr_fixture *fixture;
    size_t i;

    fixture = calloc(1, sizeof(*fixture));
    assert_non_null(fixture);

    memset(&fixture->device, 0, sizeof(fixture->device));
    memset(&fixture->unit, 0, sizeof(fixture->unit));
    memset(&fixture->mux, 0, sizeof(fixture->mux));
    memset(&fixture->lnorder, 0, sizeof(fixture->lnorder));
    memset(&fixture->lines, 0, sizeof(fixture->lines));

    fixture->device.name = "TMXR";
    fixture->mux.dptr = &fixture->device;
    fixture->mux.ldsc = fixture->lines;
    fixture->mux.lines = 4;
    fixture->mux.uptr = &fixture->unit;
    fixture->mux.lnorder = fixture->lnorder;

    for (i = 0; i < fixture->mux.lines; i++) {
        fixture->lines[i].mp = &fixture->mux;
        fixture->lines[i].dptr = &fixture->device;
        fixture->lines[i].rxbsz = 16;
        fixture->lines[i].txbsz = 16;
    }

    *state = fixture;
    return 0;
}

static int teardown_sim_tmxr_fixture(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    size_t i;

    for (i = 0; i < fixture->mux.lines; i++) {
        if (fixture->lines[i].txlogref != NULL)
            sim_close_logfile(&fixture->lines[i].txlogref);
        free(fixture->lines[i].ipad);
        free(fixture->lines[i].port);
        free(fixture->lines[i].acl);
        free(fixture->lines[i].txlogname);
        free(fixture->lines[i].rxb);
        free(fixture->lines[i].rbr);
        free(fixture->lines[i].txb);
        free(fixture->lines[i].rxpb);
        free(fixture->lines[i].txpb);
        free(fixture->lines[i].serconfig);
        free(fixture->lines[i].destination);
        free(fixture->lines[i].lpb);
    }
    free(fixture->unit.filename);
    free(fixture);
    *state = NULL;
    return 0;
}

/* Verify loopback toggling updates the line flag and allocates or frees
   the private loopback buffer. */
static void test_tmxr_loopback_toggle_updates_buffer_state(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    TMLN *line = &fixture->lines[0];

    assert_false(tmxr_get_line_loopback(line));
    assert_null(line->lpb);

    assert_int_equal(tmxr_set_line_loopback(line, TRUE), SCPE_OK);
    assert_true(tmxr_get_line_loopback(line));
    assert_non_null(line->lpb);
    assert_int_equal(line->lpbsz, line->rxbsz);
    assert_true(line->ser_connect_pending);

    assert_int_equal(tmxr_set_line_loopback(line, FALSE), SCPE_OK);
    assert_false(tmxr_get_line_loopback(line));
    assert_null(line->lpb);
    assert_int_equal(line->lpbsz, 0);
}

/* Verify half-duplex mode toggles independently of other line state. */
static void test_tmxr_halfduplex_toggle_round_trips(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    TMLN *line = &fixture->lines[0];

    assert_false(tmxr_get_line_halfduplex(line));
    assert_int_equal(tmxr_set_line_halfduplex(line, TRUE), SCPE_OK);
    assert_true(tmxr_get_line_halfduplex(line));
    assert_int_equal(tmxr_set_line_halfduplex(line, FALSE), SCPE_OK);
    assert_false(tmxr_get_line_halfduplex(line));
}

/* Verify receive and transmit queue helpers account for both linear and
   wrapped circular-buffer state. */
static void test_tmxr_queue_length_helpers_handle_wraparound(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    TMLN *line = &fixture->lines[0];

    line->rxbpr = 2;
    line->rxbpi = 6;
    assert_int_equal(tmxr_rqln(line), 4);
    assert_int_equal(tmxr_input_pending_ln(line), 4);

    line->rxbpr = 10;
    line->rxbpi = 3;
    assert_int_equal(tmxr_rqln(line), 9);
    assert_int_equal(tmxr_input_pending_ln(line), -7);

    line->txbpr = 5;
    line->txbpi = 9;
    assert_int_equal(tmxr_tqln(line), 4);

    line->txbpr = 12;
    line->txbpi = 3;
    assert_int_equal(tmxr_tqln(line), 7);

    line->txppsize = 20;
    line->txppoffset = 5;
    assert_int_equal(tmxr_tpqln(line), 15);
    assert_true(tmxr_tpbusyln(line));

    line->txppoffset = line->txppsize;
    assert_int_equal(tmxr_tpqln(line), 0);
    assert_false(tmxr_tpbusyln(line));
}

/* Verify poll-interval validation rejects zero and stores accepted
   values on the multiplexer descriptor. */
static void test_tmxr_connection_poll_interval_validates_zero(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;

    assert_int_equal(tmxr_connection_poll_interval(&fixture->mux, 0), SCPE_ARG);
    assert_int_equal(tmxr_connection_poll_interval(&fixture->mux, 5), SCPE_OK);
    assert_int_equal(fixture->mux.poll_interval, 5);
}

/* Verify an unconfigured line reports no attach-string state. */
static void test_tmxr_line_attach_string_returns_null_for_unconfigured_line(
    void **state)
{
    struct sim_tmxr_fixture *fixture = *state;

    assert_null(tmxr_line_attach_string(&fixture->lines[0]));
}

/* Verify listener-style line configuration is rendered with the expected
   line prefix, option overrides, ACL entries, log file, and loopback state. */
static void test_tmxr_line_attach_string_formats_listener_state(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    TMLN *line = &fixture->lines[1];
    char *attach;

    fixture->mux.buffered = 64;
    line->conn = TMXR_LINE_DISABLED;
    line->modem_control = TRUE;
    line->txbfd = TRUE;
    line->txbsz = 128;
    line->notelnet = TRUE;
    line->nomessage = TRUE;
    line->loopback = TRUE;
    line->port = strdup("5000");
    line->acl = strdup("+127.0.0.1/32,-10.0.0.0/8");
    line->txlogname = strdup("tmxr.log");

    assert_non_null(line->port);
    assert_non_null(line->acl);
    assert_non_null(line->txlogname);

    attach = tmxr_line_attach_string(line);
    assert_non_null(attach);
    assert_string_equal(
        attach,
        "Line=1,Disabled,Modem,Buffered=128,5000;notelnet;nomessage;"
        "Accept=127.0.0.1/32;Reject=10.0.0.0/8,Log=tmxr.log,Loopback");
    free(attach);
}

/* Verify outgoing serial configuration is rendered using the serial port
   name and non-default line settings. */
static void test_tmxr_line_attach_string_formats_serial_destination(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    TMLN *line = &fixture->lines[0];
    char *attach;

    fixture->mux.lines = 1;
    line->destination = strdup("ttyUSB0;ignored");
    line->serconfig = strdup("19200-7E1");
    line->serport = (SERHANDLE)(uintptr_t)1;

    assert_non_null(line->destination);
    assert_non_null(line->serconfig);

    attach = tmxr_line_attach_string(line);
    assert_non_null(attach);
    assert_string_equal(attach, ",Connect=ttyUSB0;19200-7E1");
    free(attach);
}

/* Verify line-order parsing preserves explicit order and fills in the
   remaining lines when ALL terminates the list. */
static void test_tmxr_set_lnorder_fills_unspecified_lines_with_all(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    char *output = NULL;
    size_t output_size = 0;
    FILE *stream;

    assert_int_equal(
        tmxr_set_lnorder(NULL, 0, "1;3;ALL", &fixture->mux), SCPE_OK);

    assert_int_equal(fixture->mux.lnorder[0], 1);
    assert_int_equal(fixture->mux.lnorder[1], 3);
    assert_int_equal(fixture->mux.lnorder[2], 0);
    assert_int_equal(fixture->mux.lnorder[3], 2);

    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(tmxr_show_lnorder(stream, NULL, 0, &fixture->mux), SCPE_OK);
    assert_int_equal(fclose(stream), 0);

    assert_non_null(output);
    assert_string_equal(output, "Order=1;3;0;2\n");
    free(output);
}

/* Verify malformed line-order input is rejected without disturbing the
   existing sequential-order marker. */
static void test_tmxr_set_lnorder_rejects_trailing_items_after_all(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;

    fixture->mux.lnorder[0] = -1;
    assert_int_equal(
        tmxr_set_lnorder(NULL, 0, "ALL;1", &fixture->mux),
        SCPE_2MARG | SCPE_NOMESSAGE);
    assert_int_equal(fixture->mux.lnorder[0], -1);
}

/* Verify line messages buffer bytes verbatim when no newline expansion is
   needed. */
static void test_tmxr_linemsg_buffers_plain_text(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    TMLN *line = &fixture->lines[0];
    char *buffered;

    ensure_line_tx_buffer(line, 64);
    line->conn = TRUE;
    line->txbfd = TRUE;
    line->notelnet = TRUE;

    tmxr_linemsg(line, "hello");

    buffered = copy_line_tx_buffer(line);
    assert_string_equal(buffered, "hello");
    free(buffered);
}

/* Verify formatted line messages expand bare LF into CRLF while preserving
   existing CRLF pairs. */
static void test_tmxr_linemsgf_expands_newlines_to_crlf(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    TMLN *line = &fixture->lines[0];
    char *buffered;

    ensure_line_tx_buffer(line, 128);
    line->conn = TRUE;
    line->txbfd = TRUE;
    line->notelnet = TRUE;

    tmxr_linemsgf(line, "one\ntwo\r\nthree");

    buffered = copy_line_tx_buffer(line);
    assert_string_equal(buffered, "one\r\ntwo\r\nthree");
    free(buffered);
}

/* Verify the varargs formatter grows beyond its stack buffer when needed and
   still emits the full formatted message. */
static void test_tmxr_linemsgf_handles_long_formatted_output(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    TMLN *line = &fixture->lines[0];
    char long_text[1024];
    char *buffered;

    memset(long_text, 'A', sizeof(long_text) - 1);
    long_text[sizeof(long_text) - 1] = '\0';

    ensure_line_tx_buffer(line, 2048);
    line->conn = TRUE;
    line->txbfd = TRUE;
    line->notelnet = TRUE;

    tmxr_linemsgf(line, "<%s>", long_text);

    buffered = copy_line_tx_buffer(line);
    assert_int_equal((int)strlen(buffered), (int)strlen(long_text) + 2);
    assert_true(buffered[0] == '<');
    assert_true(buffered[strlen(buffered) - 1] == '>');
    free(buffered);
}

/* Verify the summary formatter distinguishes between single-line and
   multi-line muxes. */
static void test_tmxr_show_summ_formats_single_and_multi_line_state(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    char *output = NULL;
    size_t output_size = 0;
    FILE *stream;

    fixture->mux.lines = 1;
    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(tmxr_show_summ(stream, NULL, 0, &fixture->mux), SCPE_OK);
    assert_int_equal(fclose(stream), 0);
    assert_string_equal(output, "disconnected");
    free(output);

    output = NULL;
    output_size = 0;
    fixture->mux.lines = 4;
    fixture->lines[1].sock = (SOCKET)1;
    fixture->lines[2].serport = (SERHANDLE)(uintptr_t)1;
    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(tmxr_show_summ(stream, NULL, 0, &fixture->mux), SCPE_OK);
    assert_int_equal(fclose(stream), 0);
    assert_string_equal(output, "2 current connections");
    free(output);
}

/* Verify show-cstat reports disconnected states and connected line stats. */
static void test_tmxr_show_cstat_formats_disconnected_and_connected_states(
    void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    char *output = NULL;
    size_t output_size = 0;
    FILE *stream;

    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(tmxr_show_cstat(stream, NULL, 0, &fixture->mux), SCPE_OK);
    assert_int_equal(fclose(stream), 0);
    assert_string_equal(output, "all disconnected\n");
    free(output);

    output = NULL;
    output_size = 0;
    fixture->lines[2].sock = (SOCKET)1;
    fixture->lines[2].ipad = strdup("127.0.0.1");
    fixture->lines[2].rcve = 1;
    fixture->lines[2].xmte = 1;
    fixture->lines[2].rxcnt = 7;
    fixture->lines[2].txcnt = 9;
    assert_non_null(fixture->lines[2].ipad);
    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(tmxr_show_cstat(stream, NULL, 0, &fixture->mux), SCPE_OK);
    assert_int_equal(fclose(stream), 0);
    assert_non_null(strstr(output, "Line 2:"));
    assert_non_null(strstr(output, "input (on)"));
    assert_non_null(strstr(output, "7"));
    assert_non_null(strstr(output, "9"));
    free(output);
}

/* Verify line logging state can be enabled, reported, and disabled, and that
   the mux attach string is regenerated for the owning unit. */
static void test_tmxr_log_configuration_updates_mux_attach_string(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    char *log_path = make_temp_log_path();
    char *output = NULL;
    size_t output_size = 0;
    FILE *stream;

    fixture->unit.filename = strdup("stale");
    assert_non_null(fixture->unit.filename);

    assert_int_equal(
        tmxr_set_log(NULL, 1, log_path, &fixture->mux), SCPE_OK);
    assert_non_null(fixture->lines[1].txlog);
    assert_non_null(fixture->lines[1].txlogname);
    assert_string_equal(fixture->lines[1].txlogname, log_path);
    assert_non_null(fixture->unit.filename);
    assert_non_null(strstr(fixture->unit.filename, "Line=1,Log="));
    assert_non_null(strstr(fixture->unit.filename, log_path));

    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(tmxr_show_log(stream, NULL, 1, &fixture->mux), SCPE_OK);
    assert_int_equal(fclose(stream), 0);
    assert_non_null(strstr(output, log_path));
    free(output);

    assert_int_equal(tmxr_set_nolog(NULL, 1, NULL, &fixture->mux), SCPE_OK);
    assert_null(fixture->lines[1].txlog);
    assert_null(fixture->lines[1].txlogname);
    assert_null(fixture->unit.filename);

    output = NULL;
    output_size = 0;
    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(tmxr_show_log(stream, NULL, 1, &fixture->mux), SCPE_OK);
    assert_int_equal(fclose(stream), 0);
    assert_string_equal(output, "no logging");
    free(output);

    unlink(log_path);
    free(log_path);
}

/* Verify attach help reflects single-line and multi-line mux characteristics
   through the device help context. */
static void test_tmxr_attach_help_formats_single_and_multi_line_modes(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    char *output = NULL;
    size_t output_size = 0;
    FILE *stream;

    fixture->device.help_ctx = &fixture->mux;

    fixture->mux.lines = 1;
    fixture->mux.port_speed_control = TRUE;
    fixture->mux.modem_control = TRUE;
    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(
        tmxr_attach_help(stream, &fixture->device, &fixture->unit, 0, NULL),
        SCPE_OK);
    assert_int_equal(fclose(stream), 0);
    assert_non_null(strstr(output, "TMXR Multiplexer Attach Help"));
    assert_non_null(strstr(output, "full modem control device"));
    assert_non_null(strstr(output, "ATTACH TMXR Connect=ser0"));
    assert_non_null(strstr(output, "SPEED=*8"));
    free(output);

    output = NULL;
    output_size = 0;
    fixture->mux.lines = 4;
    fixture->mux.port_speed_control = FALSE;
    fixture->mux.modem_control = FALSE;
    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(
        tmxr_attach_help(stream, &fixture->device, &fixture->unit, 0, NULL),
        SCPE_OK);
    assert_int_equal(fclose(stream), 0);
    assert_non_null(strstr(output, "Line buffering for all 4 lines"));
    assert_non_null(strstr(output, "ATTACH TMXR Line=n,Loopback"));
    assert_non_null(strstr(output, "ATTACH TMXR Line=n,Disable"));
    assert_non_null(strstr(output, "Valid line numbers are from 0 thru 3"));
    free(output);
}

/* Verify the line connection-order display handles both the sequential
   sentinel and compressed explicit ranges. */
static void test_tmxr_show_lnorder_formats_sequential_and_ranges(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    char *output = NULL;
    size_t output_size = 0;
    FILE *stream;

    fixture->mux.lnorder[0] = -1;
    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(tmxr_show_lnorder(stream, NULL, 0, &fixture->mux), SCPE_OK);
    assert_int_equal(fclose(stream), 0);
    assert_string_equal(output, "Order=0-3\n");
    free(output);

    output = NULL;
    output_size = 0;
    fixture->mux.lnorder[0] = 0;
    fixture->mux.lnorder[1] = 1;
    fixture->mux.lnorder[2] = 3;
    fixture->mux.lnorder[3] = -1;
    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(tmxr_show_lnorder(stream, NULL, 0, &fixture->mux), SCPE_OK);
    assert_int_equal(fclose(stream), 0);
    assert_string_equal(output, "Order=0-1;3\n");
    free(output);
}

/* Verify show-lnorder reports undefined connection-order support. */
static void test_tmxr_show_lnorder_rejects_missing_array(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;

    fixture->mux.lnorder = NULL;
    assert_int_equal(tmxr_show_lnorder(stdout, NULL, 0, &fixture->mux), SCPE_NXPAR);
}

/* Verify connection reporting covers outgoing network, incoming network,
   and serial cases without requiring a live host socket. */
static void test_tmxr_fconns_formats_connecting_and_serial_states(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    char *output = NULL;
    size_t output_size = 0;
    FILE *stream;

    fixture->lines[0].connecting = (SOCKET)1;
    fixture->lines[0].destination = strdup("remote:1234");
    fixture->lines[0].cnms = 1;
    assert_non_null(fixture->lines[0].destination);
    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    tmxr_fconns(stream, &fixture->lines[0], 0);
    assert_int_equal(fclose(stream), 0);
    assert_non_null(strstr(output, "line 0: "));
    assert_non_null(strstr(output, "Connection to remote port remote:1234"));
    assert_non_null(strstr(output, "Connecting"));
    free(output);

    output = NULL;
    output_size = 0;
    memset(&fixture->lines[1], 0, sizeof(fixture->lines[1]));
    fixture->lines[1].mp = &fixture->mux;
    fixture->lines[1].dptr = &fixture->device;
    fixture->lines[1].rxbsz = 16;
    fixture->lines[1].txbsz = 16;
    fixture->lines[1].connecting = (SOCKET)1;
    fixture->lines[1].ipad = strdup("127.0.0.1");
    assert_non_null(fixture->lines[1].ipad);
    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    tmxr_fconns(stream, &fixture->lines[1], 1);
    assert_int_equal(fclose(stream), 0);
    assert_non_null(strstr(output, "Connection from IP address 127.0.0.1"));
    free(output);

    output = NULL;
    output_size = 0;
    memset(&fixture->lines[2], 0, sizeof(fixture->lines[2]));
    fixture->lines[2].mp = &fixture->mux;
    fixture->lines[2].dptr = &fixture->device;
    fixture->lines[2].rxbsz = 16;
    fixture->lines[2].txbsz = 16;
    fixture->lines[2].serport = (SERHANDLE)(uintptr_t)1;
    fixture->lines[2].destination = strdup("ser0");
    fixture->lines[2].modem_control = TRUE;
    fixture->lines[2].modembits = TMXR_MDM_DTR | TMXR_MDM_CTS;
    assert_non_null(fixture->lines[2].destination);
    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    tmxr_fconns(stream, &fixture->lines[2], 2);
    assert_int_equal(fclose(stream), 0);
    assert_non_null(strstr(output, "Connected to serial port ser0"));
    assert_non_null(strstr(output, "Modem Bits: DTR CTS"));
    free(output);
}

/* Verify statistics reporting covers connected counters and buffered
   accounting. */
static void test_tmxr_fstats_formats_connected_counters(void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    TMLN *line = &fixture->lines[0];
    char *output = NULL;
    size_t output_size = 0;
    FILE *stream;

    line->connecting = (SOCKET)1;
    line->rcve = 1;
    line->xmte = 1;
    line->rxbpr = 1;
    line->rxbpi = 4;
    line->rxcnt = 7;
    line->rxpcnt = 2;
    ensure_line_tx_buffer(line, 64);
    line->txbpr = 0;
    line->txbpi = 5;
    line->txcnt = 9;
    line->txpcnt = 3;
    line->txppsize = 8;
    line->txppoffset = 3;
    line->txbfd = TRUE;
    line->txbsz = 64;
    line->txdrp = 4;
    line->txstall = 5;
    line->rxbps = 9600;
    line->txbps = 9600;
    line->bpsfactor = 2.0;

    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    tmxr_fstats(stream, line, 0);
    assert_int_equal(fclose(stream), 0);
    assert_non_null(strstr(output, "Line 0:"));
    assert_non_null(strstr(output, "input (on) queued/total = 1/7"));
    assert_non_null(strstr(output, "packets = 2"));
    assert_non_null(strstr(output, "output (on) queued/total = 5/9"));
    assert_non_null(strstr(output, "packet data queued/packets sent = 5/3"));
    assert_non_null(strstr(output, "speed = 9600*2 bps"));
    assert_non_null(strstr(output, "output buffer size = 64"));
    assert_non_null(strstr(output, "bytes in buffer = 5"));
    assert_non_null(strstr(output, "dropped = 4"));
    assert_non_null(strstr(output, "stalled = 5"));
    free(output);
}

/* Verify mux-wide Telnet mode toggles propagate to all lines and reject
   changes while a line is attached. */
static void test_tmxr_notelnet_mode_toggles_and_rejects_attached_lines(
    void **state)
{
    struct sim_tmxr_fixture *fixture = *state;

    assert_int_equal(tmxr_set_notelnet(&fixture->mux), SCPE_OK);
    assert_true(fixture->mux.notelnet);
    assert_true(fixture->lines[0].notelnet);
    assert_true(fixture->lines[3].notelnet);

    assert_int_equal(tmxr_clear_notelnet(&fixture->mux), SCPE_OK);
    assert_false(fixture->mux.notelnet);
    assert_false(fixture->lines[0].notelnet);
    assert_false(fixture->lines[3].notelnet);

    fixture->lines[2].connecting = (SOCKET)(uintptr_t)1;
    assert_int_equal(tmxr_set_notelnet(&fixture->mux), SCPE_ALATT);
    assert_false(fixture->mux.notelnet);
}

/* Verify mux-wide connect-message mode toggles propagate to all lines and
   reject changes while a line is attached. */
static void test_tmxr_nomessage_mode_toggles_and_rejects_attached_lines(
    void **state)
{
    struct sim_tmxr_fixture *fixture = *state;

    assert_int_equal(tmxr_set_nomessage(&fixture->mux), SCPE_OK);
    assert_true(fixture->mux.nomessage);
    assert_true(fixture->lines[0].nomessage);
    assert_true(fixture->lines[3].nomessage);

    assert_int_equal(tmxr_clear_nomessage(&fixture->mux), SCPE_OK);
    assert_false(fixture->mux.nomessage);
    assert_false(fixture->lines[0].nomessage);
    assert_false(fixture->lines[3].nomessage);

    fixture->lines[1].serport = (SERHANDLE)(uintptr_t)1;
    assert_int_equal(tmxr_set_nomessage(&fixture->mux), SCPE_ALATT);
    assert_false(fixture->mux.nomessage);
}

/* Verify port-speed-control state changes are allowed before attach and
   rejected once the mux is attached.  Line-specific setters must also
   validate the requested line. */
static void test_tmxr_port_speed_control_setters_update_expected_state(
    void **state)
{
    struct sim_tmxr_fixture *fixture = *state;
    char *output = NULL;
    size_t output_size = 0;
    FILE *stream;

    assert_int_equal(tmxr_set_port_speed_control(&fixture->mux), SCPE_OK);
    assert_true(fixture->mux.port_speed_control);
    assert_true(fixture->lines[0].port_speed_control);
    assert_true(fixture->lines[3].port_speed_control);

    assert_int_equal(tmxr_clear_port_speed_control(&fixture->mux), SCPE_OK);
    assert_false(fixture->mux.port_speed_control);
    assert_false(fixture->lines[0].port_speed_control);
    assert_false(fixture->lines[3].port_speed_control);

    assert_int_equal(tmxr_set_line_port_speed_control(&fixture->mux, 2), SCPE_OK);
    assert_true(fixture->lines[2].port_speed_control);
    assert_int_equal(
        tmxr_clear_line_port_speed_control(&fixture->mux, 2), SCPE_OK);
    assert_false(fixture->lines[2].port_speed_control);

    assert_int_equal(
        tmxr_set_line_port_speed_control(&fixture->mux, 4),
        SCPE_ARG | SCPE_NOMESSAGE);

    fixture->unit.flags |= UNIT_ATT;

    assert_int_equal(
        tmxr_set_port_speed_control(&fixture->mux),
        SCPE_ALATT | SCPE_NOMESSAGE);
    fixture->unit.flags &= ~UNIT_ATT;
    assert_int_equal(tmxr_set_port_speed_control(&fixture->mux), SCPE_OK);
    fixture->unit.flags |= UNIT_ATT;
    assert_int_equal(
        tmxr_clear_port_speed_control(&fixture->mux),
        SCPE_ALATT | SCPE_NOMESSAGE);
    assert_int_equal(
        tmxr_set_line_port_speed_control(&fixture->mux, 2),
        SCPE_ALATT | SCPE_NOMESSAGE);
    assert_int_equal(
        tmxr_clear_line_port_speed_control(&fixture->mux, 2),
        SCPE_ALATT | SCPE_NOMESSAGE);

    stream = open_memstream(&output, &output_size);
    assert_non_null(stream);
    assert_int_equal(tmxr_show_lines(stream, NULL, 0, &fixture->mux), SCPE_OK);
    assert_int_equal(fclose(stream), 0);
    assert_string_equal(output, "lines=4");
    free(output);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(
            test_tmxr_loopback_toggle_updates_buffer_state,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(test_tmxr_halfduplex_toggle_round_trips,
                                        setup_sim_tmxr_fixture,
                                        teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_queue_length_helpers_handle_wraparound,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_connection_poll_interval_validates_zero,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_line_attach_string_returns_null_for_unconfigured_line,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_line_attach_string_formats_listener_state,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_line_attach_string_formats_serial_destination,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_set_lnorder_fills_unspecified_lines_with_all,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_set_lnorder_rejects_trailing_items_after_all,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(test_tmxr_linemsg_buffers_plain_text,
                                        setup_sim_tmxr_fixture,
                                        teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_linemsgf_expands_newlines_to_crlf,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_linemsgf_handles_long_formatted_output,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_show_summ_formats_single_and_multi_line_state,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_show_cstat_formats_disconnected_and_connected_states,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_log_configuration_updates_mux_attach_string,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_attach_help_formats_single_and_multi_line_modes,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_show_lnorder_formats_sequential_and_ranges,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_show_lnorder_rejects_missing_array,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_fconns_formats_connecting_and_serial_states,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_fstats_formats_connected_counters,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_notelnet_mode_toggles_and_rejects_attached_lines,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_nomessage_mode_toggles_and_rejects_attached_lines,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
        cmocka_unit_test_setup_teardown(
            test_tmxr_port_speed_control_setters_update_expected_state,
            setup_sim_tmxr_fixture, teardown_sim_tmxr_fixture),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
