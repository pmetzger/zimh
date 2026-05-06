/* hp2100_sys_internal.h: HP 2100 system internal declarations */

#ifndef HP2100_SYS_INTERNAL_H_
#define HP2100_SYS_INTERNAL_H_

#include <stdbool.h>

#include "hp2100_defs.h"
#include "hp2100_cpu.h"

static inline bool hp2100_instruction_has_clear_flag(t_value instruction)
{
    return (instruction & IR_HCF) != 0;
}

#endif /* HP2100_SYS_INTERNAL_H_ */
