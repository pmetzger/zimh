/* i1401_bool_internal.h: IBM 1401 boolean conversion helpers */

#ifndef I1401_BOOL_INTERNAL_H_
#define I1401_BOOL_INTERNAL_H_

#include <stdbool.h>

#include "i1401_defs.h"

static inline bool i1401_set_modifier_enabled(int32 value)
{
    return value != 0;
}

static inline bool i1401_mask_is_set(uint32 value, uint32 mask)
{
    return (value & mask) != 0;
}

static inline bool i1401_fortran_conversion_switch_requested(int32 switches)
{
    return i1401_mask_is_set((uint32)switches, SWMASK('F'));
}

#endif /* I1401_BOOL_INTERNAL_H_ */
