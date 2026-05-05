/* sim_tape_internal.h: Internal tape helpers for ZIMH */
// SPDX-FileCopyrightText: 2026 The ZIMH Project
// SPDX-License-Identifier: MIT

#ifndef SIM_TAPE_INTERNAL_H_
#define SIM_TAPE_INTERNAL_H_ 0

#include "sim_defs.h"

typedef struct ANSI_VOL1 {
    char type[3];       /* VOL  */
    char num;           /* 1    */
    char ident[6];      /* <ansi <a> characters blank padded > */
    char accessibity;   /* blank */
    char reserved1[13]; /*      */
    char implement[13]; /*      */
    char owner[14];     /*      */
    char reserved2[28]; /*      */
    char standard;      /* 1,3 or 4  */
} ANSI_VOL1;

typedef struct ANSI_HDR1 {     /* Also EOF1, EOV1 */
    char type[3];              /* HDR|EOF|EOV  */
    char num;                  /* 1    */
    char file_ident[17];       /* filename */
    char file_set[6];          /* label ident */
    char file_section[4];      /* 0001 */
    char file_sequence[4];     /* 0001 */
    char generation_number[4]; /* 0001 */
    char version_number[2];    /* 00 */
    char creation_date[6];     /* cyyddd */
    char expiration_date[6];
    char accessibility;   /* space */
    char block_count[6];  /* 000000 */
    char system_code[13]; /* */
    char reserved[7];     /* blank */
} ANSI_HDR1;

typedef struct ANSI_HDR2 { /* Also EOF2, EOV2 */
    char type[3];          /* HDR  */
    char num;              /* 2    */
    char record_format;    /* F(fixed)|D(variable)|S(spanned) */
    char block_length[5];  /* label ident */
    char record_length[5]; /*  */
    char reserved_os1[21]; /* */
    char carriage_control; /* A - Fortran CC, M - Record contained CC */
    char reserved_os2[13]; /* */
    char buffer_offset[2]; /* */
    char reserved_std[28]; /* */
} ANSI_HDR2;

typedef struct ANSI_HDR3 {   /* Also EOF3, EOV3 */
    char type[3];            /* HDR  */
    char num;                /* 3    */
    char rms_attributes[64]; /* 32 bytes of RMS attributes as hex */
    char reserved[12];       /* */
} ANSI_HDR3;

typedef struct ANSI_HDR4 {   /* Also EOF4, EOV4 */
    char type[3];            /* HDR  */
    char num;                /* 4    */
    char blank;              /* blank */
    char extra_name[62];     /*  */
    char extra_name_used[2]; /* 99 */
    char unused[11];
} ANSI_HDR4;

/* Fill a DOS11 fallback filename field using the six-digit file count. */
void sim_tape_dos11_fallback_name(char name[9], uint32 file_count);

/* Format a zero-padded decimal value into a fixed-width ANSI label field. */
t_bool sim_tape_format_ansi_decimal(char *field, size_t field_size,
                                    uint32 value);

#endif
