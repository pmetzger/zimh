#include "test_simh_personality.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "scp.h"

#define SIMH_TEST_MAX_DEVICES 16

DEVICE *sim_devices[SIMH_TEST_MAX_DEVICES];
REG *sim_PC = NULL;
char sim_name[64] = "simbase-unit";
const char *sim_stop_messages[SCPE_BASE];
int32 sim_emax = 0;

void simh_test_reset_simulator_state(void)
{
    memset(sim_devices, 0, sizeof(sim_devices));
    sim_PC = NULL;
    memset(sim_name, 0, sizeof(sim_name));
    memcpy(sim_name, "simbase-unit", sizeof("simbase-unit"));
    memset(sim_stop_messages, 0, sizeof(sim_stop_messages));
    sim_emax = 0;
}

int simh_test_set_sim_name(const char *name)
{
    size_t length;

    length = strlen(name);
    if (length >= sizeof(sim_name)) {
        errno = ENAMETOOLONG;
        return -1;
    }

    memset(sim_name, 0, sizeof(sim_name));
    memcpy(sim_name, name, length + 1);
    return 0;
}

int simh_test_set_devices(DEVICE *const *devices)
{
    size_t index;

    memset(sim_devices, 0, sizeof(sim_devices));

    for (index = 0; devices[index] != NULL; ++index) {
        if (index >= SIMH_TEST_MAX_DEVICES - 1) {
            errno = E2BIG;
            return -1;
        }
        sim_devices[index] = devices[index];
    }

    sim_devices[index] = NULL;
    return 0;
}

t_stat sim_instr(void)
{
    return SCPE_OK;
}

t_stat sim_load(FILE *ptr, CONST char *cptr, CONST char *fnam, int flag)
{
    (void)ptr;
    (void)cptr;
    (void)fnam;
    (void)flag;

    return SCPE_OK;
}

t_stat fprint_sym(FILE *ofile, t_addr addr, t_value *val, UNIT *uptr, int32 sw)
{
    (void)ofile;
    (void)addr;
    (void)val;
    (void)uptr;
    (void)sw;

    return SCPE_OK;
}

t_stat parse_sym(CONST char *cptr, t_addr addr, UNIT *uptr, t_value *val,
                 int32 sw)
{
    (void)cptr;
    (void)addr;
    (void)uptr;
    (void)val;
    (void)sw;

    return SCPE_OK;
}
