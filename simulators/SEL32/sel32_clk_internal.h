/* sel32_clk_internal.h: internal SEL-32 clock helpers */

#ifndef SEL32_CLK_INTERNAL_H_
#define SEL32_CLK_INTERNAL_H_ 0

#include <math.h>

#include "sel32_defs.h"

#define SEL32_ITM_COUNT_MODULUS 4294967296.0

/* Convert remaining interval-timer microseconds to a 32-bit timer count. */
static inline uint32 sel32_itm_remaining_counts(double usecs,
                                                int32 tick_size_x_100)
{
    double counts;
    double wrapped_counts;

    if (!isfinite(usecs) || (usecs <= 0.0) || (tick_size_x_100 <= 0))
        return 0;

    counts = (100.0 * usecs) / tick_size_x_100;
    wrapped_counts = fmod(counts, SEL32_ITM_COUNT_MODULUS);

    return (uint32)wrapped_counts;
}

#endif
