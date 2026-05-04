#include <string.h>

#include "test_cmocka.h"

#include "sim_defs.h"
#include "nd100_defs.h"

#define ND100_SHT_AD 0000600
#define ND100_SHT_RIGHT_32 0000040

extern int ins_sht(int IR, int off);
extern REG cpu_reg[];
extern uint16 RBLK[16][8];

uint16 PM[65536];
uint16 M[65536];
REG *sim_PC = &cpu_reg[0];
DEVICE *sim_devices[] = {&cpu_dev, NULL};
char sim_name[] = "ND100";
const char *sim_stop_messages[SCPE_BASE];
int32 sim_emax = 1;

static void reset_cpu_registers(void)
{
    memset(R, 0, sizeof(R));
    memset(RBLK, 0, sizeof(RBLK));
    regSTH = 0;
    curlvl = 0;
}

static void test_sht_ad_right_32_uses_full_count(void **state)
{
    (void)state;

    reset_cpu_registers();
    regA = 0x8000;
    regD = 0x0000;

    assert_int_equal(ins_sht(ND_SHT | ND100_SHT_AD | ND100_SHT_RIGHT_32, 0),
                     SCPE_OK);

    assert_int_equal(regA, 0xffff);
    assert_int_equal(regD, 0xffff);
}

uint16 prdmem(int addr, int how)
{
    (void)addr;
    (void)how;
    return 0;
}

void pwrmem(int addr, int val, int how)
{
    (void)addr;
    (void)val;
    (void)how;
}

uint16 rdmem(int addr, int how)
{
    (void)addr;
    (void)how;
    return 0;
}

void wrmem(int addr, int val, int how)
{
    (void)addr;
    (void)val;
    (void)how;
}

uint8 rdbyte(int vaddr, int lr, int how)
{
    (void)vaddr;
    (void)lr;
    (void)how;
    return 0;
}

void wrbyte(int vaddr, int val, int lr, int how)
{
    (void)vaddr;
    (void)val;
    (void)lr;
    (void)how;
}

void mm_privcheck(void) {}

void mm_wrpcr(void) {}

int mm_tra(int reg)
{
    (void)reg;
    return SCPE_OK;
}

int iox_clk(int dev)
{
    (void)dev;
    return SCPE_OK;
}

int iox_floppy(int dev)
{
    (void)dev;
    return SCPE_OK;
}

int iox_tty(int dev)
{
    (void)dev;
    return SCPE_OK;
}

t_stat sim_load(FILE *ptr, const char *cptr, const char *fnam, int flag)
{
    (void)ptr;
    (void)cptr;
    (void)fnam;
    (void)flag;
    return SCPE_OK;
}

t_stat fprint_sym(FILE *ofile, t_addr addr, t_value *val, UNIT *uptr, int32 sw)
{
    (void)ofile;
    (void)addr;
    (void)val;
    (void)uptr;
    (void)sw;
    return SCPE_OK;
}

t_stat parse_sym(const char *cptr, t_addr addr, UNIT *uptr, t_value *val,
                 int32 sw)
{
    (void)cptr;
    (void)addr;
    (void)uptr;
    (void)val;
    (void)sw;
    return SCPE_OK;
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_sht_ad_right_32_uses_full_count),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
