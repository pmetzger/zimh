#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <cmocka.h>

#include "vax_defs.h"
#include "sim_scsi.h"

#define RZ_INT_ILLCMD 0x40
#define RZ_INT_FC 0x08

extern DEVICE rz_dev;
extern UNIT rz_unit[];
extern SCSI_BUS rz_bus;
extern uint8 rz_cfg1;
extern uint8 rz_cfg2;
extern uint8 rz_cfg3;
extern uint8 rz_dest;
extern uint8 rz_int;
extern uint8 rz_seq;
extern uint8 rz_stat;
extern uint8 rz_fifo[];
extern uint32 rz_fifo_b;
extern uint32 rz_fifo_c;
extern uint32 rz_fifo_t;
extern uint32 rz_txc;

void rz_cmd(uint32 cmd);

int32 int_req[IPL_HLVL];
int32 sys_model;
uint32 trpirq;
uint32 fault_PC;
int32 hlt_pin;
jmp_buf save_env;

int32 Map_ReadB(uint32 ba, int32 bc, uint8 *buf)
{
    (void)ba;
    memset(buf, 0, (size_t)bc);
    return 0;
}

int32 Map_WriteB(uint32 ba, int32 bc, uint8 *buf)
{
    (void)ba;
    (void)bc;
    (void)buf;
    return 0;
}

int32 eval_int(void)
{
    return 0;
}

static void reset_rz_command_state(void)
{
    memset(&rz_bus, 0, sizeof(rz_bus));
    rz_bus.dptr = &rz_dev;
    rz_bus.initiator = -1;
    rz_bus.target = -1;
    rz_cfg1 = RZ_SCSI_ID;
    rz_int = 0;
    rz_fifo_t = 0;
    rz_fifo_b = 0;
    rz_fifo_c = 0;
    memset(rz_fifo, 0, 16);
    int_req[IPL_SC] = 0;
    sim_cancel(&rz_unit[8]);
}

static void test_flush_fifo_does_not_set_interrupt(void **state)
{
    (void)state;

    reset_rz_command_state();
    rz_fifo[0] = 0xa5;
    rz_fifo_b = 1;
    rz_fifo_c = 1;

    rz_cmd(0x01);

    assert_int_equal(rz_fifo_c, 0);
    assert_false(rz_int & RZ_INT_FC);
    assert_false(int_req[IPL_SC] != 0);
}

static void test_disable_selection_reselection_completes(void **state)
{
    (void)state;

    reset_rz_command_state();

    rz_cmd(0x45);

    assert_true(rz_int & RZ_INT_FC);
    assert_false(rz_int & RZ_INT_ILLCMD);
}

static void test_reset_chip_disconnects_without_bus_reset(void **state)
{
    (void)state;

    reset_rz_command_state();
    rz_bus.initiator = RZ_SCSI_ID;
    rz_bus.target = 4;
    rz_bus.phase = SCSI_MSGI;
    rz_bus.atn = TRUE;
    rz_bus.req = TRUE;
    rz_bus.buf_b = 2;
    rz_bus.buf_t = 4;
    rz_bus.sense_key = 0x05;
    rz_bus.sense_code = 0x24;
    rz_bus.sense_qual = 0x01;
    rz_bus.sense_info = 0x12345678;

    rz_txc = 3;
    rz_cfg1 = 0x5a;
    rz_cfg2 = 0xa5;
    rz_cfg3 = 0xc3;
    rz_stat = 0xff;
    rz_seq = 0x12;
    rz_int = RZ_INT_FC;
    rz_dest = 4;
    rz_fifo[0] = 0xa5;
    rz_fifo_b = 1;
    rz_fifo_c = 1;

    rz_cmd(0x02);

    assert_int_equal(rz_txc, 0);
    assert_int_equal(rz_cfg1, 0x02);
    assert_int_equal(rz_cfg2, 0);
    assert_int_equal(rz_cfg3, 0);
    assert_int_equal(rz_stat, 0);
    assert_int_equal(rz_seq, 0);
    assert_int_equal(rz_int, 0);
    assert_int_equal(rz_dest, 0);
    assert_int_equal(rz_fifo_c, 0);
    assert_false(int_req[IPL_SC] != 0);

    assert_int_equal(rz_bus.phase, SCSI_DATO);
    assert_int_equal(rz_bus.initiator, -1);
    assert_int_equal(rz_bus.target, -1);
    assert_false(rz_bus.atn);
    assert_false(rz_bus.req);
    assert_int_equal(rz_bus.buf_b, 0);
    assert_int_equal(rz_bus.buf_t, 0);
    assert_int_equal(rz_bus.sense_key, 0x05);
    assert_int_equal(rz_bus.sense_code, 0x24);
    assert_int_equal(rz_bus.sense_qual, 0x01);
    assert_int_equal(rz_bus.sense_info, 0x12345678);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_flush_fifo_does_not_set_interrupt),
        cmocka_unit_test(test_disable_selection_reselection_completes),
        cmocka_unit_test(test_reset_chip_disconnects_without_bus_reset),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
