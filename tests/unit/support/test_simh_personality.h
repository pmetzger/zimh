#ifndef TEST_SIMH_PERSONALITY_H
#define TEST_SIMH_PERSONALITY_H

#include "sim_defs.h"

void simh_test_reset_simulator_state(void);
int simh_test_set_sim_name(const char *name);
int simh_test_set_devices(DEVICE *const *devices);

#endif
