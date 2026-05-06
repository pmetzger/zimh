#include <stdint.h>

typedef int8_t sint8;
typedef int16_t sint16;
typedef int32_t sint32;
typedef int64_t sint64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#include "milieu.h"

#ifndef NMI_FALSE
#error "milieu.h must define NMI_FALSE"
#endif

#ifndef NMI_TRUE
#error "milieu.h must define NMI_TRUE"
#endif

#ifdef FALSE
#error "milieu.h must not define generic FALSE"
#endif

#ifdef TRUE
#error "milieu.h must not define generic TRUE"
#endif

int main(void)
{
    return (NMI_FALSE == 0 && NMI_TRUE == 1) ? 0 : 1;
}
