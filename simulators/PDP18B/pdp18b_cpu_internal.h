/* pdp18b_cpu_internal.h: internal helpers for 18-bit PDP CPU tests */

#ifndef PDP18B_CPU_INTERNAL_H_
#define PDP18B_CPU_INTERNAL_H_ 0

#include "pdp18b_defs.h"

enum { PDP18B_LAC_WIDTH = 19 };

/* Rotate a PDP-18B link-plus-AC bit pattern within its 19-bit field. */
static inline uint32
pdp18b_lac_rotate_left(uint32 lac, unsigned int count)
{
    uint32 value = lac & LACMASK;

    count %= PDP18B_LAC_WIDTH;
    if (count == 0)
        return value;
    return ((value << count) | (value >> (PDP18B_LAC_WIDTH - count))) &
           LACMASK;
}

/* Rotate a PDP-18B link-plus-AC bit pattern within its 19-bit field. */
static inline uint32
pdp18b_lac_rotate_right(uint32 lac, unsigned int count)
{
    uint32 value = lac & LACMASK;

    count %= PDP18B_LAC_WIDTH;
    if (count == 0)
        return value;
    return ((value >> count) | (value << (PDP18B_LAC_WIDTH - count))) &
           LACMASK;
}

#endif
