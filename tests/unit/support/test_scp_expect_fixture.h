#ifndef TEST_SCP_EXPECT_FIXTURE_H
#define TEST_SCP_EXPECT_FIXTURE_H

#include "sim_defs.h"
#include "scp_expect.h"
#include "sim_tmxr.h"

struct scp_expect_fixture {
    DEVICE device;
    UNIT unit;
    TMXR mux;
    TMLN lines[2];
    EXPECT exp;
    SEND snd;
};

int simh_test_setup_scp_expect_fixture(void **state);
int simh_test_teardown_scp_expect_fixture(void **state);

#endif
