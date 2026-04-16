#include "sim_defs.h"
#include "scp.h"
#include "sim_video.h"

/* Keep the actual process entrypoint out of scp.c so shared control
   code can be linked into host-side tests without a duplicate main(). */
int main(int argc, char *argv[])
{
    return scp_main(argc, argv);
}
