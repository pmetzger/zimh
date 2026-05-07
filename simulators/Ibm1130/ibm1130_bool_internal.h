/* ibm1130_bool_internal.h: IBM 1130 boolean conversion helpers */

#ifndef IBM1130_BOOL_INTERNAL_H_
#define IBM1130_BOOL_INTERNAL_H_

#include <stdbool.h>

#include "sim_defs.h"

static inline bool ibm1130_mask_is_set(uint32 value, uint32 mask)
{
    return (value & mask) != 0;
}

static inline bool ibm1130_switch_requested(int32 switches, int32 mask)
{
    return ibm1130_mask_is_set((uint32)switches, (uint32)mask);
}

static inline bool ibm1130_any_state_mask_is_set(uint32 state0, uint32 state1,
                                                 uint32 state2, uint32 mask)
{
    return ibm1130_mask_is_set(state0 | state1 | state2, mask);
}

#endif /* IBM1130_BOOL_INTERNAL_H_ */
