/* isbc206_internal.h: Intel iSBC 206 private controller state */
// SPDX-FileCopyrightText: 2017 William A. Beech
// SPDX-License-Identifier: X11

#ifndef ISBC206_INTERNAL_H_
#define ISBC206_INTERNAL_H_

#include "system_defs.h"

#define ISBC206_HDD_NUM 2

typedef struct { // HDD definition
    int t0;
    int rdy;
    uint8 sec;
    uint8 cyl;
} HDDDEF;

typedef struct {                // HDC definition
    uint8 baseport;             // HDC base port
    uint8 intnum;               // interrupt number
    uint8 verb;                 // verbose flag
    uint16 iopb;                // HDC IOPB
    uint8 stat;                 // HDC status
    uint8 rdychg;               // HDC ready change
    uint8 rtype;                // HDC result type
    uint8 rbyte0;               // HDC result byte for type 00
    uint8 rbyte1;               // HDC result byte for type 10
    uint8 intff;                // HDC interrupt FF
    HDDDEF hd[ISBC206_HDD_NUM]; // indexed by the HDD number
} HDCDEF;

extern HDCDEF hdc206;

#endif /* ISBC206_INTERNAL_H_ */
