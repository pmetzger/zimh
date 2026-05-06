#define MUSASHI_CNF "test_m68k_minimal_conf.h"

#include "m68k.h"

#ifndef NMI_FALSE
#error "m68k.h must define NMI_FALSE"
#endif

#ifndef NMI_TRUE
#error "m68k.h must define NMI_TRUE"
#endif

#ifdef FALSE
#error "m68k.h must not define generic FALSE"
#endif

#ifdef TRUE
#error "m68k.h must not define generic TRUE"
#endif

int main(void)
{
    return (NMI_FALSE == 0 && NMI_TRUE == 1) ? 0 : 1;
}
