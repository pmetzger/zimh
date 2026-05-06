/* hp3000_lp_internal.h: HP 3000 line printer internal declarations */

#ifndef HP3000_LP_INTERNAL_H_
#define HP3000_LP_INTERNAL_H_

#include <stdbool.h>

#include "hp3000_defs.h"

static inline bool hp3000_lp_device_command_out(FLIP_FLOP device_command,
                                                bool invert_device_command)
{
    return ((device_command == SET) != invert_device_command);
}

#endif /* HP3000_LP_INTERNAL_H_ */
