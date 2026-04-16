/* scp_context.h: shared SCP simulator context helpers

   These declarations cover the small slice of SCP state and helper
   logic that describes the active simulator context: default device and
   unit pointers, internal device registration, and device/unit naming
   and lookup helpers.  Keeping these declarations together makes the
   boundary between generic SCP helpers and the command loop easier to
   follow.
*/

#ifndef SIM_SCP_CONTEXT_H_
#define SIM_SCP_CONTEXT_H_ 0

#include "sim_defs.h"

extern DEVICE *sim_dfdev;
extern UNIT *sim_dfunit;
extern DEVICE **sim_internal_devices;
extern uint32 sim_internal_device_count;

const char *sim_dname(DEVICE *dptr);
const char *sim_uname(UNIT *uptr);
const char *sim_set_uname(UNIT *uptr, const char *uname);
DEVICE *find_dev(const char *cptr);
DEVICE *find_unit(const char *cptr, UNIT **uptr);
t_stat sim_register_internal_device(DEVICE *dptr);
t_bool qdisable(DEVICE *dptr);

#endif
