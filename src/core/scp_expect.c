/* scp_expect.c: SCP expect/send subsystem

   This file contains the SCP support for queued console input injection
   and output-triggered expect rules.  The subsystem owns the SEND/EXPECT
   command handlers, the expect rule matcher, and the internal device used
   to stop simulation when an expect rule fires.
*/

#include "sim_defs.h"
#include "scp.h"
#include "sim_tmxr.h"

/* Return the default SEND/EXPECT parameter value for one line or console. */
static uint32 get_default_env_parameter(const char *dev_name,
                                        const char *param_name,
                                        uint32 default_value)
{
    char varname[CBUFSIZE];
    unsigned long val;
    char *endptr;
    const char *colon = strchr(dev_name, ':');
    char *env_val;

    if (colon)
        snprintf(varname, sizeof(varname), "%s_%*.*s_%s", param_name,
                 (int)(colon - dev_name), (int)(colon - dev_name), dev_name,
                 colon + 1);
    else
        snprintf(varname, sizeof(varname), "%s_%s", param_name, dev_name);

    env_val = getenv(varname);

    if (NULL == env_val)
        val = default_value;
    else {
        val = strtoul(env_val, &endptr, 0);
        if (*endptr)
            val = default_value;
    }

    /* This narrows from unsigned long to uint32 by design. */
    return (uint32)val;
}

/* Persist one default SEND/EXPECT parameter in the host environment. */
static void set_default_env_parameter(const char *dev_name,
                                      const char *param_name, uint32 value)
{
    char varname[CBUFSIZE];
    char valbuf[CBUFSIZE];
    const char *colon = strchr(dev_name, ':');

    if (colon)
        snprintf(varname, sizeof(varname), "%s_%*.*s_%s", param_name,
                 (int)(colon - dev_name), (int)(colon - dev_name), dev_name,
                 colon + 1);
    else
        snprintf(varname, sizeof(varname), "%s_%s", param_name, dev_name);

    snprintf(valbuf, sizeof(valbuf), "%u", value);
    setenv(varname, valbuf, 1);
}

/* Describe the internal expect device in SHOW output. */
static const char *sim_int_expect_description(DEVICE *dptr)
{
    (void)dptr;
    return "Expect facility";
}

/* Stop simulation when an expect rule fires. */
static t_stat expect_svc(UNIT *uptr)
{
    (void)uptr;
    return SCPE_EXPECT | (scp_do_echo_enabled() ? 0 : SCPE_NOMESSAGE);
}

static UNIT sim_expect_unit = {UDATA(&expect_svc, 0, 0)};

DEVICE sim_expect_dev = {"INT-EXPECT",
                         &sim_expect_unit,
                         NULL,
                         NULL,
                         1,
                         0,
                         0,
                         0,
                         0,
                         0,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         DEV_NOSAVE,
                         0,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         sim_int_expect_description};

t_bool sim_expect_is_unit(const UNIT *uptr)
{
    return uptr == &sim_expect_unit;
}

/* Search one expect context for a matching display-format pattern. */
static CONST EXPTAB *sim_exp_fnd(CONST EXPECT *exp, const char *match,
                                 size_t start_rule)
{
    size_t i;

    if (NULL == exp->rules)
        return NULL;

    for (i = start_rule; i < exp->size; i++)
        if (!strcmp(exp->rules[i].match_pattern, match))
            return &exp->rules[i];

    return NULL;
}

/* Remove one expect rule from a context and compact the table. */
static t_stat sim_exp_clr_tab(EXPECT *exp, EXPTAB *ep)
{
    size_t i;

    if (NULL == ep)
        return SCPE_OK;

    free(ep->match);
    free(ep->match_pattern);
    free(ep->act);
#if defined(USE_REGEX)
    if (ep->switches & EXP_TYP_REGEX)
        pcre_free(ep->regex);
#endif
    exp->size -= 1;
    for (i = ep - exp->rules; i < exp->size; i++)
        exp->rules[i] = exp->rules[i + 1];
    if (exp->size == 0) {
        free(exp->rules);
        exp->rules = NULL;
    }
    return SCPE_OK;
}

/* Display one expect rule in command-replay form. */
static t_stat sim_exp_show_tab(FILE *st, const EXPECT *exp, const EXPTAB *ep)
{
    const char *dev_name = tmxr_expect_line_name(exp);
    uint32 default_haltafter =
        get_default_env_parameter(dev_name, "SIM_EXPECT_HALTAFTER", 0);

    if (!ep)
        return SCPE_OK;
    fprintf(st, "    EXPECT");
    if (ep->switches & EXP_TYP_PERSIST)
        fprintf(st, " -p");
    if (ep->switches & EXP_TYP_CLEARALL)
        fprintf(st, " -c");
    if (ep->switches & EXP_TYP_REGEX)
        fprintf(st, " -r");
    if (ep->switches & EXP_TYP_REGEX_I)
        fprintf(st, " -i");
    if (ep->after != default_haltafter)
        fprintf(st, " HALTAFTER=%d", (int)ep->after);
    fprintf(st, " %s", ep->match_pattern);
    if (ep->cnt > 0)
        fprintf(st, " [%d]", ep->cnt);
    if (ep->act)
        fprintf(st, " %s", ep->act);
    fprintf(st, "\n");
    return SCPE_OK;
}

t_stat send_cmd(int32 flag, CONST char *cptr)
{
    char gbuf[CBUFSIZE];
    CONST char *tptr;
    uint8 dbuf[CBUFSIZE];
    uint32 dsize = 0;
    const char *dev_name;
    uint32 delay;
    t_bool delay_set = FALSE;
    uint32 after;
    t_bool after_set = FALSE;
    t_stat r;
    SEND *snd;

    GET_SWITCHES(cptr);
    tptr = get_glyph(cptr, gbuf, ',');
    if (sim_isalpha(gbuf[0]) && (strchr(gbuf, ':'))) {
        r = tmxr_locate_line_send(gbuf, &snd);
        if (r != SCPE_OK)
            return r;
        cptr = tptr;
        tptr = get_glyph(tptr, gbuf, ',');
    } else
        snd = sim_cons_get_send();
    dev_name = tmxr_send_line_name(snd);
    if (!flag)
        return sim_send_clear(snd);
    delay = get_default_env_parameter(dev_name, "SIM_SEND_DELAY",
                                      SEND_DEFAULT_DELAY);
    after = get_default_env_parameter(dev_name, "SIM_SEND_AFTER", delay);
    while (*cptr) {
        if ((!strncmp(gbuf, "DELAY=", 6)) && (gbuf[6])) {
            delay = (uint32)get_uint(&gbuf[6], 10, 2000000000, &r);
            if (r != SCPE_OK)
                return sim_messagef(SCPE_ARG, "Invalid Delay Value: %s\n",
                                    &gbuf[6]);
            cptr = tptr;
            tptr = get_glyph(cptr, gbuf, ',');
            delay_set = TRUE;
            if (!after_set)
                after = delay;
            continue;
        }
        if ((!strncmp(gbuf, "AFTER=", 6)) && (gbuf[6])) {
            after = (uint32)get_uint(&gbuf[6], 10, 2000000000, &r);
            if (r != SCPE_OK)
                return sim_messagef(SCPE_ARG, "Invalid After Value: %s\n",
                                    &gbuf[6]);
            cptr = tptr;
            tptr = get_glyph(cptr, gbuf, ',');
            after_set = TRUE;
            continue;
        }
        if ((*cptr == '"') || (*cptr == '\''))
            break;
        return SCPE_ARG;
    }
    if (!*cptr) {
        if ((!delay_set) && (!after_set))
            return SCPE_2FARG;
        set_default_env_parameter(dev_name, "SIM_SEND_DELAY", delay);
        set_default_env_parameter(dev_name, "SIM_SEND_AFTER", after);
        return SCPE_OK;
    }
    if ((*cptr != '"') && (*cptr != '\''))
        return sim_messagef(SCPE_ARG, "String must be quote delimited\n");
    cptr = get_glyph_quoted(cptr, gbuf, 0);
    if (*cptr != '\0')
        return SCPE_2MARG;

    if (SCPE_OK != sim_decode_quoted_string(gbuf, dbuf, &dsize))
        return sim_messagef(SCPE_ARG, "Invalid String\n");
    return sim_send_input(snd, dbuf, dsize, after, delay);
}

t_stat sim_show_send(FILE *st, DEVICE *dptr, UNIT *uptr, int32 flag,
                     CONST char *cptr)
{
    char gbuf[CBUFSIZE];
    CONST char *tptr;
    t_stat r;
    SEND *snd;

    (void)dptr;
    (void)uptr;
    (void)flag;

    tptr = get_glyph(cptr, gbuf, ',');
    if (sim_isalpha(gbuf[0]) && (strchr(gbuf, ':'))) {
        r = tmxr_locate_line_send(gbuf, &snd);
        if (r != SCPE_OK)
            return r;
        cptr = tptr;
    } else
        snd = sim_cons_get_send();
    if (*cptr)
        return SCPE_2MARG;
    return sim_show_send_input(st, snd);
}

t_stat expect_cmd(int32 flag, CONST char *cptr)
{
    char gbuf[CBUFSIZE];
    CONST char *tptr;
    t_stat r;
    EXPECT *exp;

    GET_SWITCHES(cptr);
    tptr = get_glyph(cptr, gbuf, ',');
    if (sim_isalpha(gbuf[0]) && (strchr(gbuf, ':'))) {
        r = tmxr_locate_line_expect(gbuf, &exp);
        if (r != SCPE_OK)
            return sim_messagef(r, "No such active line: %s\n", gbuf);
        cptr = tptr;
    } else
        exp = sim_cons_get_expect();
    if (flag)
        return sim_set_expect(exp, cptr);
    return sim_set_noexpect(exp, cptr);
}

t_stat sim_show_expect(FILE *st, DEVICE *dptr, UNIT *uptr, int32 flag,
                       CONST char *cptr)
{
    char gbuf[CBUFSIZE];
    CONST char *tptr;
    t_stat r;
    EXPECT *exp;

    (void)dptr;
    (void)uptr;
    (void)flag;

    tptr = get_glyph(cptr, gbuf, ',');
    if (sim_isalpha(gbuf[0]) && (strchr(gbuf, ':'))) {
        r = tmxr_locate_line_expect(gbuf, &exp);
        if (r != SCPE_OK)
            return r;
        cptr = tptr;
    } else
        exp = sim_cons_get_expect();
    if (*cptr && (*cptr != '"') && (*cptr != '\''))
        return SCPE_ARG;
    tptr = get_glyph_quoted(cptr, gbuf, 0);
    if (*tptr != '\0')
        return SCPE_2MARG;
    if (*cptr && (cptr[strlen(cptr) - 1] != '"') &&
        (cptr[strlen(cptr) - 1] != '\''))
        return SCPE_ARG;
    return sim_exp_show(st, exp, gbuf);
}

t_stat sim_set_expect(EXPECT *exp, CONST char *cptr)
{
    char gbuf[CBUFSIZE];
    CONST char *tptr;
    CONST char *c1ptr;
    const char *dev_name;
    uint32 after;
    t_bool after_set = FALSE;
    int32 cnt = 0;
    t_stat r;

    if ((cptr == NULL) || (*cptr == 0))
        return SCPE_2FARG;
    dev_name = tmxr_expect_line_name(exp);
    after = get_default_env_parameter(dev_name, "SIM_EXPECT_HALTAFTER", 0);
    if (*cptr == '[') {
        cnt = (int32)strtotv(cptr + 1, &c1ptr, 10);
        if ((cptr == c1ptr) || (*c1ptr != ']') || (cnt <= 0))
            return sim_messagef(SCPE_ARG,
                                "Invalid Repeat count specification\n");
        cnt -= 1;
        cptr = c1ptr + 1;
        while (sim_isspace(*cptr))
            ++cptr;
    }
    tptr = get_glyph(cptr, gbuf, ',');
    if ((!strncmp(gbuf, "HALTAFTER=", 10)) && (gbuf[10])) {
        after = (uint32)get_uint(&gbuf[10], 10, 2000000000, &r);
        if (r != SCPE_OK)
            return sim_messagef(SCPE_ARG, "Invalid Halt After Value: %s\n",
                                &gbuf[10]);
        cptr = tptr;
        after_set = TRUE;
    }
    if ((*cptr != '\0') && (*cptr != '"') && (*cptr != '\''))
        return sim_messagef(SCPE_ARG, "String must be quote delimited\n");
    cptr = get_glyph_quoted(cptr, gbuf, 0);

    if ((gbuf[0] == '\0') && (*cptr == '\0') && after_set) {
        set_default_env_parameter(dev_name, "SIM_EXPECT_HALTAFTER", after);
        return SCPE_OK;
    }

    return sim_exp_set(exp, gbuf, cnt, after, sim_switches, cptr);
}

t_stat sim_set_noexpect(EXPECT *exp, const char *cptr)
{
    char gbuf[CBUFSIZE];

    if (!cptr || !*cptr)
        return sim_exp_clrall(exp);
    if ((*cptr != '"') && (*cptr != '\''))
        return sim_messagef(SCPE_ARG, "String must be quote delimited\n");
    cptr = get_glyph_quoted(cptr, gbuf, 0);
    if (*cptr != '\0')
        return SCPE_2MARG;
    return sim_exp_clr(exp, gbuf);
}

t_stat sim_exp_clr(EXPECT *exp, const char *match)
{
    EXPTAB *ep = (EXPTAB *)sim_exp_fnd(exp, match, 0);

    while (NULL != ep) {
        sim_exp_clr_tab(exp, ep);
        ep = (EXPTAB *)sim_exp_fnd(exp, match, ep - exp->rules);
    }
    return SCPE_OK;
}

t_stat sim_exp_clrall(EXPECT *exp)
{
    size_t i;

    for (i = 0; i < exp->size; i++) {
        free(exp->rules[i].match);
        free(exp->rules[i].match_pattern);
        free(exp->rules[i].act);
#if defined(USE_REGEX)
        if (exp->rules[i].switches & EXP_TYP_REGEX)
            pcre_free(exp->rules[i].regex);
#endif
    }
    free(exp->rules);
    exp->rules = NULL;
    exp->size = 0;
    free(exp->buf);
    exp->buf = NULL;
    exp->buf_size = 0;
    exp->buf_data = exp->buf_ins = 0;
    return SCPE_OK;
}

t_stat sim_exp_set(EXPECT *exp, const char *match, int32 cnt, uint32 after,
                   int32 switches, const char *act)
{
    EXPTAB *ep;
    uint8 *match_buf;
    uint32 match_size;
    size_t i;

    match_buf = (uint8 *)calloc(strlen(match) + 1, 1);
    if (!match_buf)
        return SCPE_MEM;
    if (switches & EXP_TYP_REGEX) {
#if !defined(USE_REGEX)
        free(match_buf);
        return sim_messagef(SCPE_ARG, "RegEx support not available\n");
#else
        pcre *re;
        const char *errmsg;
        int erroffset, re_nsub;

        memcpy(match_buf, match + 1, strlen(match) - 2);
        match_buf[strlen(match) - 2] = '\0';
        re = pcre_compile((char *)match_buf,
                          (switches & EXP_TYP_REGEX_I) ? PCRE_CASELESS : 0,
                          &errmsg, &erroffset, NULL);
        if (re == NULL) {
            sim_messagef(SCPE_ARG, "Regular Expression Error: %s\n", errmsg);
            free(match_buf);
            return SCPE_ARG | SCPE_NOMESSAGE;
        }
        (void)pcre_fullinfo(re, NULL, PCRE_INFO_CAPTURECOUNT, &re_nsub);
        sim_debug(exp->dbit, exp->dptr,
                  "Expect Regular Expression: \"%s\" has %d sub "
                  "expressions\n",
                  match_buf, re_nsub);
        pcre_free(re);
#endif
    } else {
        if (switches & EXP_TYP_REGEX_I) {
            free(match_buf);
            return sim_messagef(
                SCPE_ARG, "Case independent matching is only valid for RegEx "
                          "expect rules.\n");
        }
        sim_data_trace(exp->dptr, exp->dptr->units, (const uint8 *)match, "",
                       strlen(match) + 1, "Expect Match String", exp->dbit);
        if (SCPE_OK !=
            sim_decode_quoted_string(match, match_buf, &match_size)) {
            free(match_buf);
            return sim_messagef(SCPE_ARG, "Invalid quoted string\n");
        }
    }
    free(match_buf);
    for (i = 0; i < exp->size; i++) {
        if ((0 == strcmp(match, exp->rules[i].match_pattern)) &&
            (exp->rules[i].switches & EXP_TYP_PERSIST))
            return sim_messagef(SCPE_ARG,
                                "Persistent Expect rule with identical match "
                                "string already exists\n");
    }
    if (after && exp->size)
        return sim_messagef(
            SCPE_ARG, "Multiple concurrent EXPECT rules aren't valid when "
                      "a HALTAFTER parameter is non-zero\n");
    exp->rules =
        (EXPTAB *)realloc(exp->rules, sizeof(*exp->rules) * (exp->size + 1));
    ep = &exp->rules[exp->size];
    exp->size += 1;
    memset(ep, 0, sizeof(*ep));
    ep->after = after;
    ep->match_pattern = (char *)malloc(strlen(match) + 1);
    if (ep->match_pattern)
        strcpy(ep->match_pattern, match);
    ep->cnt = cnt;
    ep->switches = switches;
    match_buf = (uint8 *)calloc(strlen(match) + 1, 1);
    if ((match_buf == NULL) || (ep->match_pattern == NULL)) {
        sim_exp_clr_tab(exp, ep);
        free(match_buf);
        return SCPE_MEM;
    }
    if (switches & EXP_TYP_REGEX) {
#if defined(USE_REGEX)
        const char *errmsg;
        int erroffset;

        memcpy(match_buf, match + 1, strlen(match) - 2);
        match_buf[strlen(match) - 2] = '\0';
        ep->regex = pcre_compile(
            (char *)match_buf, (switches & EXP_TYP_REGEX_I) ? PCRE_CASELESS : 0,
            &errmsg, &erroffset, NULL);
        (void)pcre_fullinfo(ep->regex, NULL, PCRE_INFO_CAPTURECOUNT,
                            &ep->re_nsub);
#endif
        free(match_buf);
        match_buf = NULL;
    } else {
        sim_data_trace(exp->dptr, exp->dptr->units, (const uint8 *)match, "",
                       strlen(match) + 1, "Expect Match String", exp->dbit);
        (void)sim_decode_quoted_string(match, match_buf, &match_size);
        ep->match = match_buf;
        ep->size = match_size;
    }
    if (ep->act) {
        free(ep->act);
        ep->act = NULL;
    }
    if (act)
        while (sim_isspace(*act))
            ++act;
    if ((act != NULL) && (*act != 0)) {
        char *newp;

        act = sim_unsub_args(act);
        newp = (char *)calloc(strlen(act) + 1, sizeof(*act));
        if (newp == NULL)
            return SCPE_MEM;
        strcpy(newp, act);
        ep->act = newp;
    }
    for (i = 0; i < exp->size; i++) {
        size_t pattern_bytes = 10 * strlen(ep->match_pattern);
        size_t compare_size =
            (exp->rules[i].switches & EXP_TYP_REGEX)
                ? ((pattern_bytes >= 1024) ? pattern_bytes : 1024)
                : exp->rules[i].size;
        if (compare_size >= exp->buf_size) {
            exp->buf = (uint8 *)realloc(exp->buf, compare_size + 2);
            exp->buf_size = compare_size + 1;
        }
    }
    return SCPE_OK;
}

t_stat sim_exp_show(FILE *st, CONST EXPECT *exp, const char *match)
{
    CONST EXPTAB *ep = (CONST EXPTAB *)sim_exp_fnd(exp, match, 0);
    const char *dev_name = tmxr_expect_line_name(exp);
    uint32 default_haltafter =
        get_default_env_parameter(dev_name, "SIM_EXPECT_HALTAFTER", 0);

    if (exp->buf_size) {
        char *bstr = sim_encode_quoted_string(exp->buf, exp->buf_ins);

        fprintf(st, "  Match Buffer Size: %" SIZE_T_FMT "d\n", exp->buf_size);
        fprintf(st, "  Buffer Insert Offset: %" SIZE_T_FMT "d\n", exp->buf_ins);
        fprintf(st, "  Buffer Contents: %s\n", bstr);
        if (default_haltafter)
            fprintf(st, "  Default HaltAfter: %u %s\n",
                    (unsigned)default_haltafter, sim_vm_interval_units);
        free(bstr);
    }
    if (exp->dptr && (exp->dbit & exp->dptr->dctrl))
        fprintf(st, "  Expect Debugging via: SET %s DEBUG%s%s\n",
                sim_dname(exp->dptr), exp->dptr->debflags ? "=" : "",
                exp->dptr->debflags ? _get_dbg_verb(exp->dbit, exp->dptr, NULL)
                                    : "");
    fprintf(st, "  Match Rules:\n");
    if (!*match)
        return sim_exp_showall(st, exp);
    if (!ep) {
        fprintf(st, "  No Rules match '%s'\n", match);
        return SCPE_ARG;
    }
    do {
        sim_exp_show_tab(st, exp, ep);
        ep = (CONST EXPTAB *)sim_exp_fnd(exp, match, 1 + (ep - exp->rules));
    } while (ep);
    return SCPE_OK;
}

t_stat sim_exp_showall(FILE *st, const EXPECT *exp)
{
    size_t i;

    for (i = 0; i < exp->size; i++)
        sim_exp_show_tab(st, exp, &exp->rules[i]);
    return SCPE_OK;
}

t_stat sim_exp_check(EXPECT *exp, uint8 data)
{
    size_t i;
    EXPTAB *ep = NULL;
    int regex_checks = 0;
    char *tstr = NULL;

    if ((!exp) || (!exp->rules))
        return SCPE_OK;

    exp->buf[exp->buf_ins++] = data;
    exp->buf[exp->buf_ins] = '\0';
    if (exp->buf_data < exp->buf_size)
        ++exp->buf_data;

    for (i = 0; i < exp->size; i++) {
        ep = &exp->rules[i];
        if (ep->switches & EXP_TYP_REGEX) {
#if defined(USE_REGEX)
            int *ovector = NULL;
            int ovector_elts;
            int rc;
            char *cbuf = (char *)exp->buf;
            static size_t sim_exp_match_sub_count = 0;

            if (tstr)
                cbuf = tstr;
            else {
                if (strlen((char *)exp->buf) != exp->buf_ins) {
                    size_t off;

                    tstr = (char *)malloc(exp->buf_ins + 1);
                    tstr[0] = '\0';
                    for (off = 0; off < exp->buf_ins;
                         off += 1 + strlen((char *)&exp->buf[off]))
                        strcpy(&tstr[strlen(tstr)], (char *)&exp->buf[off]);
                    cbuf = tstr;
                }
            }
            ++regex_checks;
            ovector_elts = 3 * (ep->re_nsub + 1);
            ovector = (int *)calloc((size_t)ovector_elts, sizeof(*ovector));
            if (sim_deb && exp->dptr && (exp->dptr->dctrl & exp->dbit)) {
                char *estr = sim_encode_quoted_string(exp->buf, exp->buf_ins);
                sim_debug(exp->dbit, exp->dptr, "Checking String: %s\n", estr);
                sim_debug(exp->dbit, exp->dptr,
                          "Against RegEx Match Rule: %s\n", ep->match_pattern);
                free(estr);
            }
            rc = pcre_exec(ep->regex, NULL, cbuf, (int)exp->buf_ins, 0,
                           PCRE_NOTBOL, ovector, ovector_elts);
            if (rc >= 0) {
                size_t j;
                char *buf = (char *)malloc(1 + exp->buf_ins);

                for (j = 0; j < (size_t)rc; j++) {
                    char env_name[32];
                    int end_offs = ovector[2 * j + 1];
                    int start_offs = ovector[2 * j];

                    sprintf(env_name, "_EXPECT_MATCH_GROUP_%d", (int)j);
                    if (start_offs >= 0 && end_offs >= start_offs) {
                        memcpy(buf, &cbuf[start_offs], end_offs - start_offs);
                        buf[end_offs - start_offs] = '\0';
                        setenv(env_name, buf, 1);
                        sim_debug(exp->dbit, exp->dptr, "%s=%s\n", env_name,
                                  buf);
                    } else {
                        sim_debug(exp->dbit, exp->dptr, "unsetenv %s\n",
                                  env_name);
                        unsetenv(env_name);
                    }
                }
                for (; j < sim_exp_match_sub_count; j++) {
                    char env_name[32];

                    sprintf(env_name, "_EXPECT_MATCH_GROUP_%d", (int)j);
                    setenv(env_name, "", 1);
                }
                sim_exp_match_sub_count = (size_t)ep->re_nsub + 1;
                free(ovector);
                free(buf);
                break;
            }
            free(ovector);
#endif
        } else {
            if (exp->buf_data < ep->size)
                continue;
            if (exp->buf_ins < ep->size) {
                if (exp->buf_ins > 0) {
                    if (sim_deb && exp->dptr &&
                        (exp->dptr->dctrl & exp->dbit)) {
                        char *estr =
                            sim_encode_quoted_string(exp->buf, exp->buf_ins);
                        char *mstr = sim_encode_quoted_string(
                            &ep->match[ep->size - exp->buf_ins], exp->buf_ins);

                        sim_debug(exp->dbit, exp->dptr,
                                  "Checking String[0:%" SIZE_T_FMT "d]: "
                                  "%s\n",
                                  exp->buf_ins, estr);
                        sim_debug(exp->dbit, exp->dptr,
                                  "Against Match Data: %s\n", mstr);
                        free(estr);
                        free(mstr);
                    }
                    if (memcmp(exp->buf, &ep->match[ep->size - exp->buf_ins],
                               exp->buf_ins))
                        continue;
                }
                if (sim_deb && exp->dptr && (exp->dptr->dctrl & exp->dbit)) {
                    char *estr = sim_encode_quoted_string(
                        &exp->buf[exp->buf_size - (ep->size - exp->buf_ins)],
                        ep->size - exp->buf_ins);
                    char *mstr = sim_encode_quoted_string(
                        ep->match, ep->size - exp->buf_ins);

                    sim_debug(exp->dbit, exp->dptr,
                              "Checking String[%" SIZE_T_FMT "d:%" SIZE_T_FMT
                              "d]: %s\n",
                              exp->buf_size - ep->size - exp->buf_ins,
                              ep->size - exp->buf_ins, estr);
                    sim_debug(exp->dbit, exp->dptr, "Against Match Data: %s\n",
                              mstr);
                    free(estr);
                    free(mstr);
                }
                if (memcmp(&exp->buf[exp->buf_size - (ep->size - exp->buf_ins)],
                           ep->match, ep->size - exp->buf_ins))
                    continue;
                break;
            } else {
                if (sim_deb && exp->dptr && (exp->dptr->dctrl & exp->dbit)) {
                    char *estr = sim_encode_quoted_string(
                        &exp->buf[exp->buf_ins - ep->size], ep->size);
                    char *mstr = sim_encode_quoted_string(ep->match, ep->size);

                    sim_debug(exp->dbit, exp->dptr,
                              "Checking String[%" SIZE_T_FMT "u:%" SIZE_T_FMT
                              "u]: %s\n",
                              exp->buf_ins - ep->size, ep->size, estr);
                    sim_debug(exp->dbit, exp->dptr, "Against Match Data: %s\n",
                              mstr);
                    free(estr);
                    free(mstr);
                }
                if (memcmp(&exp->buf[exp->buf_ins - ep->size], ep->match,
                           ep->size))
                    continue;
                break;
            }
        }
    }
    if (exp->buf_ins == exp->buf_size) {
        if (regex_checks) {
            memmove(exp->buf, &exp->buf[exp->buf_size / 2],
                    exp->buf_size - (exp->buf_size / 2));
            exp->buf_ins -= exp->buf_size / 2;
            exp->buf_data = exp->buf_ins;
            sim_debug(exp->dbit, exp->dptr,
                      "Buffer Full - sliding the last %" SIZE_T_FMT
                      "d bytes to start of buffer new insert at: %" SIZE_T_FMT
                      "d\n",
                      exp->buf_size / 2, exp->buf_ins);
        } else {
            exp->buf_ins = 0;
            sim_debug(exp->dbit, exp->dptr, "Buffer wrapping\n");
        }
    }
    if (i != exp->size) {
        sim_debug(exp->dbit, exp->dptr, "Matched expect pattern: %s\n",
                  ep->match_pattern);
        setenv("_EXPECT_MATCH_PATTERN", ep->match_pattern, 1);
        if (ep->cnt > 0) {
            ep->cnt -= 1;
            sim_debug(exp->dbit, exp->dptr,
                      "Waiting for %d more match%s before stopping\n", ep->cnt,
                      (ep->cnt == 1) ? "" : "es");
        } else {
            uint32 after = ep->after;
            int32 switches = ep->switches;

            if (ep->act && *ep->act)
                sim_debug(exp->dbit, exp->dptr, "Initiating actions: %s\n",
                          ep->act);
            else
                sim_debug(exp->dbit, exp->dptr,
                          "No actions specified, stopping...\n");
            sim_brk_setact(ep->act);
            if (ep->switches & EXP_TYP_CLEARALL)
                sim_exp_clrall(exp);
            else if (!(ep->switches & EXP_TYP_PERSIST))
                sim_exp_clr_tab(exp, ep);
            sim_activate(
                &sim_expect_unit,
                (switches & EXP_TYP_TIME)
                    ? (int32)((sim_timer_inst_per_sec() * after) / 1000000.0)
                    : after);
        }
        exp->buf_data = exp->buf_ins = 0;
    }
    free(tstr);
    return SCPE_OK;
}

t_stat sim_send_input(SEND *snd, uint8 *data, size_t size, uint32 after,
                      uint32 delay)
{
    if (snd->extoff != 0) {
        if (snd->insoff > snd->extoff)
            memmove(snd->buffer, snd->buffer + snd->extoff,
                    snd->insoff - snd->extoff);
        snd->insoff -= snd->extoff;
        snd->extoff = 0;
    }
    if (snd->insoff + size > snd->bufsize) {
        snd->bufsize = snd->insoff + size;
        snd->buffer = (uint8 *)realloc(snd->buffer, snd->bufsize);
    }
    memcpy(snd->buffer + snd->insoff, data, size);
    snd->insoff += size;
    snd->delay = (sim_switches & SWMASK('T'))
                     ? (uint32)((sim_timer_inst_per_sec() * delay) / 1000000.0)
                     : delay;
    snd->after = (sim_switches & SWMASK('T'))
                     ? (uint32)((sim_timer_inst_per_sec() * after) / 1000000.0)
                     : after;
    if (sim_switches & SWMASK('T'))
        sim_debug(snd->dbit, snd->dptr,
                  "%d bytes queued for input. Delay %d usecs = %d insts, "
                  "After %d usecs = %d insts\n",
                  (int)size, (int)delay, (int)snd->delay, (int)after,
                  (int)snd->after);
    else
        sim_debug(snd->dbit, snd->dptr,
                  "%d bytes queued for input. Delay=%d, After=%d\n", (int)size,
                  (int)delay, (int)after);
    snd->next_time = sim_gtime() + snd->after;
    return SCPE_OK;
}

t_stat sim_send_clear(SEND *snd)
{
    snd->insoff = 0;
    snd->extoff = 0;
    return SCPE_OK;
}

t_stat sim_show_send_input(FILE *st, const SEND *snd)
{
    const char *dev_name = tmxr_send_line_name(snd);
    uint32 delay = get_default_env_parameter(dev_name, "SIM_SEND_DELAY",
                                             SEND_DEFAULT_DELAY);
    uint32 after = get_default_env_parameter(dev_name, "SIM_SEND_AFTER", delay);

    fprintf(st, "%s\n", tmxr_send_line_name(snd));
    if (snd->extoff < snd->insoff) {
        fprintf(st, "  %" SIZE_T_FMT "d bytes of pending input Data:\n    ",
                snd->insoff - snd->extoff);
        fprint_buffer_string(st, snd->buffer + snd->extoff,
                             snd->insoff - snd->extoff);
        fprintf(st, "\n");
    } else
        fprintf(st, "  No Pending Input Data\n");
    if ((snd->next_time - sim_gtime()) > 0) {
        if (((snd->next_time - sim_gtime()) >
             (sim_timer_inst_per_sec() / 1000000.0)) &&
            ((sim_timer_inst_per_sec() / 1000000.0) > 0.0))
            fprintf(st,
                    "  Minimum of %d %s (%d microseconds) before sending "
                    "first character\n",
                    (int)(snd->next_time - sim_gtime()), sim_vm_interval_units,
                    (int)((snd->next_time - sim_gtime()) /
                          (sim_timer_inst_per_sec() / 1000000.0)));
        else
            fprintf(st, "  Minimum of %d %s before sending first character\n",
                    (int)(snd->next_time - sim_gtime()), sim_vm_interval_units);
    }
    if ((snd->delay > (sim_timer_inst_per_sec() / 1000000.0)) &&
        ((sim_timer_inst_per_sec() / 1000000.0) > 0.0))
        fprintf(st, "  Minimum of %d %s (%d microseconds) between characters\n",
                (int)snd->delay, sim_vm_interval_units,
                (int)(snd->delay / (sim_timer_inst_per_sec() / 1000000.0)));
    else
        fprintf(st, "  Minimum of %d %s between characters\n", (int)snd->delay,
                sim_vm_interval_units);
    if (after)
        fprintf(st, "  Default delay before first character input is %u %s\n",
                after, sim_vm_interval_units);
    if (delay)
        fprintf(st, "  Default delay between character input is %u %s\n", after,
                sim_vm_interval_units);
    if (snd->dptr && (snd->dbit & snd->dptr->dctrl))
        fprintf(st, "  Send Debugging via: SET %s DEBUG%s%s\n",
                sim_dname(snd->dptr), snd->dptr->debflags ? "=" : "",
                snd->dptr->debflags ? _get_dbg_verb(snd->dbit, snd->dptr, NULL)
                                    : "");
    return SCPE_OK;
}

t_bool sim_send_poll_data(SEND *snd, t_stat *stat)
{
    if ((NULL != snd) && (snd->extoff < snd->insoff)) {
        if (sim_gtime() < snd->next_time) {
            *stat = SCPE_OK;
            sim_debug(snd->dbit, snd->dptr, "Too soon to inject next byte\n");
        } else {
            char dstr[8] = "";

            *stat = snd->buffer[snd->extoff++] | SCPE_KFLAG;
            snd->next_time = sim_gtime() + snd->delay;
            if (sim_isgraph(*stat & 0xFF) || ((*stat & 0xFF) == ' '))
                sprintf(dstr, " '%c'", *stat & 0xFF);
            sim_debug(snd->dbit, snd->dptr, "Byte value: 0x%02X%s injected\n",
                      *stat & 0xFF, dstr);
        }
        return TRUE;
    }
    return FALSE;
}
