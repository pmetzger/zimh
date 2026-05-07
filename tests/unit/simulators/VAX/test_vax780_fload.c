#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "test_cmocka.h"

#include "vax_defs.h"

extern bool rtfile_read(uint32 block, uint32 count, uint16 *buffer);
extern uint32 rtfile_find(uint32 block, uint32 sector);

UNIT fl_unit;
UNIT cpu_unit;
uint32 *M;
uint32 mchk_va;
uint32 mchk_ref;

void WriteIO(uint32 pa, int32 val, int32 lnt)
{
    (void)pa;
    (void)val;
    (void)lnt;
    fail_msg("rtfile_read should not write I/O space");
}

void WriteReg(uint32 pa, int32 val, int32 lnt)
{
    (void)pa;
    (void)val;
    (void)lnt;
    fail_msg("rtfile_read should not write register space");
}

static void load_sector(uint8 *disk, uint32 block, uint32 sector, uint8 seed)
{
    uint32 pos;
    uint32 i;

    pos = rtfile_find(block, sector);
    for (i = 0; i < 128; i++)
        disk[pos + i] = (uint8)(seed + i);
}

static void test_rtfile_read_decodes_little_endian_words(void **state)
{
    uint8 disk[8192];
    uint16 buffer[256];

    (void)state;

    memset(disk, 0, sizeof(disk));
    memset(buffer, 0, sizeof(buffer));
    load_sector(disk, 0, 0, 0x10);
    load_sector(disk, 0, 1, 0x20);
    load_sector(disk, 0, 2, 0x30);
    load_sector(disk, 0, 3, 0x40);
    fl_unit.filebuf = disk;
    fl_unit.capac = sizeof(disk);

    assert_true(rtfile_read(0, 1, buffer));

    assert_int_equal(buffer[0], 0x1110);
    assert_int_equal(buffer[63], 0x8f8e);
    assert_int_equal(buffer[64], 0x2120);
    assert_int_equal(buffer[127], 0x9f9e);
    assert_int_equal(buffer[128], 0x3130);
    assert_int_equal(buffer[191], 0xafae);
    assert_int_equal(buffer[192], 0x4140);
    assert_int_equal(buffer[255], 0xbfbe);
}

static void test_rtfile_read_rejects_blocks_past_capacity(void **state)
{
    uint8 disk[4096];
    uint16 buffer[256];

    (void)state;

    fl_unit.filebuf = disk;
    fl_unit.capac = rtfile_find(0, 0) + 128;

    assert_false(rtfile_read(0, 1, buffer));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_rtfile_read_decodes_little_endian_words),
        cmocka_unit_test(test_rtfile_read_rejects_blocks_past_capacity),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
