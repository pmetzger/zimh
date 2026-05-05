#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "test_cmocka.h"

#include "kx10_defs.h"
#include "sim_tmxr.h"

static unsigned int ks10_dup_debug_bits_calls;
static uint32 ks10_dup_debug_bits_reason;
static BITFIELD *ks10_dup_debug_bits_fields;
static uint32 ks10_dup_debug_bits_before;
static uint32 ks10_dup_debug_bits_after;

/*
 * Capture register bit-diff tracing so tests can characterize debug-only
 * behavior without depending on simulator debug output formatting.
 */
static void ks10_dup_record_debug_bits(uint32 dbits, DEVICE *dptr,
                                       BITFIELD *bitdefs, uint32 before,
                                       uint32 after, int terminate)
{
    (void)dptr;
    (void)terminate;

    ++ks10_dup_debug_bits_calls;
    ks10_dup_debug_bits_reason = dbits;
    ks10_dup_debug_bits_fields = bitdefs;
    ks10_dup_debug_bits_before = before;
    ks10_dup_debug_bits_after = after;
}

#define sim_debug_bits ks10_dup_record_debug_bits
#include "ks10_dup.c"
#undef sim_debug_bits

int32 tmxr_poll = 10000;

static TMLN ks10_dup_test_lines[NUM_DEVS_DUP];

t_stat uba_set_addr(UNIT *uptr, int32 val, const char *cptr, void *desc)
{
    (void)uptr;
    (void)val;
    (void)cptr;
    (void)desc;
    return SCPE_OK;
}

t_stat uba_show_addr(FILE *st, UNIT *uptr, int32 val, const void *desc)
{
    (void)st;
    (void)uptr;
    (void)val;
    (void)desc;
    return SCPE_OK;
}

t_stat uba_set_br(UNIT *uptr, int32 val, const char *cptr, void *desc)
{
    (void)uptr;
    (void)val;
    (void)cptr;
    (void)desc;
    return SCPE_OK;
}

t_stat uba_show_br(FILE *st, UNIT *uptr, int32 val, const void *desc)
{
    (void)st;
    (void)uptr;
    (void)val;
    (void)desc;
    return SCPE_OK;
}

t_stat uba_set_vect(UNIT *uptr, int32 val, const char *cptr, void *desc)
{
    (void)uptr;
    (void)val;
    (void)cptr;
    (void)desc;
    return SCPE_OK;
}

t_stat uba_show_vect(FILE *st, UNIT *uptr, int32 val, const void *desc)
{
    (void)st;
    (void)uptr;
    (void)val;
    (void)desc;
    return SCPE_OK;
}

t_stat uba_set_ctl(UNIT *uptr, int32 val, const char *cptr, void *desc)
{
    (void)uptr;
    (void)val;
    (void)cptr;
    (void)desc;
    return SCPE_OK;
}

t_stat uba_show_ctl(FILE *st, UNIT *uptr, int32 val, const void *desc)
{
    (void)st;
    (void)uptr;
    (void)val;
    (void)desc;
    return SCPE_OK;
}

void uba_set_irq(DIB *dibp, int vect)
{
    (void)dibp;
    (void)vect;
}

void uba_clr_irq(DIB *dibp, int vect)
{
    (void)dibp;
    (void)vect;
}

static void reset_dup_state(void)
{
    memset(dup_rxcsr, 0, sizeof(dup_rxcsr));
    memset(dup_rxdbuf, 0, sizeof(dup_rxdbuf));
    memset(dup_parcsr, 0, sizeof(dup_parcsr));
    memset(dup_txcsr, 0, sizeof(dup_txcsr));
    memset(dup_txdbuf, 0, sizeof(dup_txdbuf));
    memset(dup_wait, 0, sizeof(dup_wait));
    memset(ks10_dup_test_lines, 0, sizeof(ks10_dup_test_lines));

    dup_desc.lines = NUM_DEVS_DUP;
    dup_desc.ldsc = dup_ldsc = ks10_dup_test_lines;
    dup_dev.ctxt = &dup_dib;
    dup_dev.flags &= ~DEV_DIS;
    for (int i = 0; i < NUM_DEVS_DUP; ++i) {
        dup_units[i] = dup_unit_template;
        dup_units[i].flags &= ~UNIT_DIS;
    }

    ks10_dup_debug_bits_calls = 0;
    ks10_dup_debug_bits_reason = 0;
    ks10_dup_debug_bits_fields = NULL;
    ks10_dup_debug_bits_before = 0;
    ks10_dup_debug_bits_after = 0;
}

/*
 * A word write to PARCSR should update only the selected line's parameter CSR
 * and leave the other DUP line untouched.
 */
static void test_dup_word_write_updates_selected_line(void **state)
{
    t_addr pa;

    (void)state;

    reset_dup_state();
    pa = dup_dib.uba_addr + IOLN_DUP + 2;

    assert_int_equal(dup_wr(&dup_dev, pa, PARCSR_M_DECMODE | 0123, WORD), 0);

    assert_int_equal(dup_parcsr[0], 0);
    assert_int_equal(dup_parcsr[1], PARCSR_M_DECMODE | 0123);
    assert_int_equal(dup_rxcsr[1], 0);
    assert_int_equal(dup_txcsr[1], 0);
}

/*
 * A byte write to a DUP register must merge with the untouched byte of the
 * previous word; this locks in the current emulator-visible write behavior.
 */
static void test_dup_byte_write_preserves_other_byte(void **state)
{
    t_addr pa;

    (void)state;

    reset_dup_state();
    dup_parcsr[1] = 0123456;
    pa = dup_dib.uba_addr + IOLN_DUP + 2;

    assert_int_equal(dup_wr(&dup_dev, pa, 0077, BYTE), 0);

    assert_int_equal(dup_parcsr[1], (0123456 & 0xFF00) | 0077);
}

/*
 * Reads should decode the line and register from the DIB base address and
 * return the selected DUP register without disturbing unrelated state.
 */
static void test_dup_read_returns_selected_register(void **state)
{
    t_addr pa;
    uint16 data = 0;

    (void)state;

    reset_dup_state();
    dup_txcsr[1] = TXCSR_M_TXDONE | TXCSR_M_TXIE;
    pa = dup_dib.uba_addr + IOLN_DUP + 4;

    assert_int_equal(dup_rd(&dup_dev, pa, &data, WORD), 0);

    assert_int_equal(data, TXCSR_M_TXDONE | TXCSR_M_TXIE);
    assert_int_equal(dup_txcsr[1], TXCSR_M_TXDONE | TXCSR_M_TXIE);
    assert_int_equal(ks10_dup_debug_bits_calls, 1);
    assert_int_equal(ks10_dup_debug_bits_reason, DEBUG_DETAIL);
    assert_ptr_equal(ks10_dup_debug_bits_fields, dup_txcsr_bits);
    assert_int_equal(ks10_dup_debug_bits_before, data);
    assert_int_equal(ks10_dup_debug_bits_after, dup_txcsr[1]);
}

/*
 * Writes should report the same before/after bit-diff tracing that reads
 * already provide. This test is expected to fail until the write-side
 * bitdefs table is used.
 */
static void test_dup_write_reports_register_bit_changes(void **state)
{
    t_addr pa;

    (void)state;

    reset_dup_state();
    dup_parcsr[1] = PARCSR_M_DECMODE;
    pa = dup_dib.uba_addr + IOLN_DUP + 2;

    assert_int_equal(dup_wr(&dup_dev, pa, 0123, WORD), 0);

    assert_int_equal(dup_parcsr[1], 0123);
    assert_int_equal(ks10_dup_debug_bits_calls, 1);
    assert_int_equal(ks10_dup_debug_bits_reason, DEBUG_DETAIL);
    assert_ptr_equal(ks10_dup_debug_bits_fields, dup_parcsr_bits);
    assert_int_equal(ks10_dup_debug_bits_before, PARCSR_M_DECMODE);
    assert_int_equal(ks10_dup_debug_bits_after, dup_parcsr[1]);
}

/*
 * The generic UBA handlers receive the active DIB through dptr->ctxt. An
 * alternate DIB base should therefore decode the DUP line relative to that
 * context, not the file-global dup_dib address.
 */
static void test_dup_handlers_decode_line_from_device_context(void **state)
{
    DEVICE dev = dup_dev;
    DIB alternate_dib = dup_dib;
    t_addr pa;
    uint16 data = 0;

    (void)state;

    reset_dup_state();
    alternate_dib.uba_addr = dup_dib.uba_addr + 01000;
    dev.ctxt = &alternate_dib;
    dev.units = dup_units;
    dup_txcsr[1] = TXCSR_M_TXDONE | TXCSR_M_TXIE;
    pa = alternate_dib.uba_addr + IOLN_DUP + 4;

    assert_int_equal(dup_rd(&dev, pa, &data, WORD), 0);

    assert_int_equal(data, TXCSR_M_TXDONE | TXCSR_M_TXIE);
}

/*
 * Writes use the same UBA handler contract as reads: the DIB supplied through
 * dptr->ctxt defines the address base for line selection.
 */
static void test_dup_write_decodes_line_from_device_context(void **state)
{
    DEVICE dev = dup_dev;
    DIB alternate_dib = dup_dib;
    t_addr pa;

    (void)state;

    reset_dup_state();
    alternate_dib.uba_addr = dup_dib.uba_addr + 01000;
    dev.ctxt = &alternate_dib;
    dev.units = dup_units;
    pa = alternate_dib.uba_addr + IOLN_DUP + 2;

    assert_int_equal(dup_wr(&dev, pa, PARCSR_M_DECMODE | 0123, WORD), 0);

    assert_int_equal(dup_parcsr[0], 0);
    assert_int_equal(dup_parcsr[1], PARCSR_M_DECMODE | 0123);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_dup_word_write_updates_selected_line),
        cmocka_unit_test(test_dup_byte_write_preserves_other_byte),
        cmocka_unit_test(test_dup_read_returns_selected_register),
        cmocka_unit_test(test_dup_write_reports_register_bit_changes),
        cmocka_unit_test(test_dup_handlers_decode_line_from_device_context),
        cmocka_unit_test(test_dup_write_decodes_line_from_device_context),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
