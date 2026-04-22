/* scp_parse.c: shared SCP token and switch parsing helpers

   This file holds the string parsing routines used by the simulator
   control program.  The command loop in scp.c still decides what the
   tokens mean, but the tokenization itself lives here so it can be
   reused and tested independently.
*/
// SPDX-FileCopyrightText: 1993-2022 Robert M Supnik
// SPDX-License-Identifier: MIT

#include "sim_defs.h"
#include "scp.h"

/* Parse one SCP token with optional case folding and quote handling. */
const char *get_glyph_gen(const char *iptr, char *optr, char mchar, t_bool uc,
                          t_bool quote, char escape_char)
{
    t_bool quoting = FALSE;
    t_bool escaping = FALSE;
    t_bool got_quoted = FALSE;
    char quote_char = 0;

    while ((*iptr != 0) && (!got_quoted) &&
           ((quote && quoting) ||
            ((sim_isspace(*iptr) == 0) && (*iptr != mchar)))) {
        if (quote) {
            if (quoting) {
                if (!escaping) {
                    if (*iptr == escape_char)
                        escaping = TRUE;
                    else if (*iptr == quote_char) {
                        quoting = FALSE;
                        got_quoted = TRUE;
                    }
                } else
                    escaping = FALSE;
            } else if ((*iptr == '"') || (*iptr == '\'')) {
                quoting = TRUE;
                quote_char = *iptr;
            }
        }
        if (sim_islower(*iptr) && uc)
            *optr = (char)sim_toupper(*iptr);
        else
            *optr = *iptr;
        iptr++;
        optr++;
    }
    if (mchar && (*iptr == mchar))
        iptr++;
    *optr = 0;
    while (sim_isspace(*iptr))
        iptr++;
    return iptr;
}

/* Parse the next token and fold alphabetic characters to upper case. */
CONST char *get_glyph(const char *iptr, char *optr, char mchar)
{
    return (CONST char *)get_glyph_gen(iptr, optr, mchar, TRUE, FALSE, 0);
}

/* Parse the next token without changing its case. */
CONST char *get_glyph_nc(const char *iptr, char *optr, char mchar)
{
    return (CONST char *)get_glyph_gen(iptr, optr, mchar, FALSE, FALSE, 0);
}

/* Parse one token, allowing it to be enclosed in quotes. */
CONST char *get_glyph_quoted(const char *iptr, char *optr, char mchar)
{
    return (CONST char *)get_glyph_gen(iptr, optr, mchar, FALSE, TRUE, '\\');
}

/* Parse the leading command token, handling SCP's special '!' form. */
CONST char *get_glyph_cmd(const char *iptr, char *optr)
{
    if ((iptr[0] == '!') && (!sim_isspace(iptr[1]))) {
        strcpy(optr, "!");
        return (CONST char *)(iptr + 1);
    }
    return (CONST char *)get_glyph_gen(iptr, optr, 0, TRUE, FALSE, 0);
}

/* Decode either symbolic switches or a numeric switch argument. */
SWITCH_PARSE get_switches(const char *cptr, int32 *sw, int32 *number)
{
    *sw = 0;
    if (*cptr != '-')
        return SW_BITMASK;
    if (number)
        *number = 0;
    if (sim_isdigit(cptr[1])) {
        char *end;
        long val = strtol(1 + cptr, &end, 10);

        if ((*end != 0) || (number == NULL))
            return SW_ERROR;
        *number = (int32)val;
        return SW_NUMBER;
    }
    for (cptr++; (sim_isspace(*cptr) == 0) && (*cptr != 0); cptr++) {
        if (sim_isalpha(*cptr) == 0)
            return SW_ERROR;
        *sw = *sw | SWMASK(sim_toupper(*cptr));
    }
    return SW_BITMASK;
}

/* Consume leading SCP simulator switches from a command string. */
CONST char *get_sim_sw(CONST char *cptr)
{
    int32 lsw, lnum;
    char gbuf[CBUFSIZE];

    while (*cptr == '-') {
        cptr = get_glyph(cptr, gbuf, 0);
        switch (get_switches(gbuf, &lsw, &lnum)) {
        case SW_ERROR:
            return NULL;
        case SW_BITMASK:
            sim_switches = sim_switches | lsw;
            break;
        case SW_NUMBER:
            sim_switch_number = lnum;
            break;
        }
    }
    return cptr;
}
