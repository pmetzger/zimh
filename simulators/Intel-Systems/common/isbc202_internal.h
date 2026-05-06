/* isbc202_internal.h: Intel iSBC 202 private controller state */
// SPDX-FileCopyrightText: 2016 William A. Beech
// SPDX-License-Identifier: X11

#ifndef ISBC202_INTERNAL_H_
#define ISBC202_INTERNAL_H_

#include "system_defs.h"

#define ISBC202_FDD_NUM 4

typedef struct { // FDD definition
    uint8 sec;
    uint8 cyl;
    uint8 att;
} FDDDEF;

typedef struct {                 // FDC definition
    uint8 baseport;              // FDC base port
    uint8 intnum;                // interrupt number
    uint8 verb;                  // verbose flag
    uint16 iopb;                 // FDC IOPB
    uint8 stat;                  // FDC status
    uint8 rdychg;                // FDC ready change
    uint8 rtype;                 // FDC result type
    uint8 rbyte0;                // FDC result byte for type 00
    uint8 rbyte1;                // FDC result byte for type 10
    uint8 intff;                 // fdc interrupt FF
    FDDDEF fdd[ISBC202_FDD_NUM]; // indexed by the FDD number
} FDCDEF;

extern FDCDEF fdc202;

#endif /* ISBC202_INTERNAL_H_ */
