/* zx200a_internal.h: Zendex ZX-200A private controller state */
// SPDX-FileCopyrightText: 2010 William A. Beech
// SPDX-License-Identifier: X11

#ifndef ZX200A_INTERNAL_H_
#define ZX200A_INTERNAL_H_

#include "system_defs.h"

#define ZX200A_FDD_NUM 6

typedef struct { // FDD definition
    uint8 sec;
    uint8 cyl;
    uint8 dd;
} FDDDEF;

typedef struct {                // FDC definition
    uint8 baseport;             // FDC base port
    uint8 intnum;               // interrupt number
    uint8 verb;                 // verbose flag
    uint16 iopb;                // FDC IOPB
    uint8 DDstat;               // FDC DD status
    uint8 SDstat;               // FDC SD status
    uint8 rdychg;               // FDC ready change
    uint8 rtype;                // FDC result type
    uint8 rbyte0;               // FDC result byte for type 00
    uint8 rbyte1;               // FDC result byte for type 10
    uint8 intff;                // fdc interrupt FF
    FDDDEF fdd[ZX200A_FDD_NUM]; // indexed by the FDD number
} FDCDEF;

extern FDCDEF zx200a;

#endif /* ZX200A_INTERNAL_H_ */
