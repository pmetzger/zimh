/* scp_unit.h: shared SCP unit attachment helpers

   These helpers were extracted from scp.c because runtime modules and
   tests need unit attachment logic without depending on the entire SCP
   command processor.
*/

#ifndef SIM_SCP_UNIT_H_
#define SIM_SCP_UNIT_H_ 0

#include "sim_defs.h"

t_stat attach_unit(UNIT *uptr, CONST char *cptr);
t_stat detach_unit(UNIT *uptr);
DEVICE *find_dev_from_unit(UNIT *uptr);

#endif
