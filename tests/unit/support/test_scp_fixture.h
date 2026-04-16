#ifndef TEST_SCP_FIXTURE_H
#define TEST_SCP_FIXTURE_H

#include "sim_defs.h"

void simh_test_init_multiunit_device(DEVICE *device, UNIT *units,
                                     uint32 numunits, const char *dev_name,
                                     const char *logical_name,
                                     uint32 dev_flags);
int simh_test_install_devices(const char *sim_name, DEVICE *const *devices);
void simh_test_free_unit_names(UNIT *units, uint32 numunits);

#endif
