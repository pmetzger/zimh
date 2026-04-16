/* scp_parse.h: shared SCP token and switch parsing helpers

   These helpers were extracted from scp.c so command parsing logic can
   be reused by smaller core modules and by host-side tests without
   dragging in the full command loop implementation.
*/

#ifndef SIM_SCP_PARSE_H_
#define SIM_SCP_PARSE_H_ 0

#include "sim_defs.h"

typedef enum { SW_ERROR, SW_BITMASK, SW_NUMBER } SWITCH_PARSE;

const char *get_glyph_gen(const char *iptr, char *optr, char mchar, t_bool uc,
                          t_bool quote, char escape_char);
CONST char *get_glyph(const char *iptr, char *optr, char mchar);
CONST char *get_glyph_nc(const char *iptr, char *optr, char mchar);
CONST char *get_glyph_quoted(const char *iptr, char *optr, char mchar);
CONST char *get_glyph_cmd(const char *iptr, char *optr);
SWITCH_PARSE get_switches(const char *cptr, int32 *sw, int32 *number);
CONST char *get_sim_sw(CONST char *cptr);

#endif
