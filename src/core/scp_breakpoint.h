/* scp_breakpoint.h: SCP breakpoint subsystem interfaces

   This header collects the breakpoint APIs and shared state extracted from
   scp.c.  The implementation manages breakpoint tables, pending actions,
   and the public status used by simulators when checking breakpoint hits.
*/
// SPDX-FileCopyrightText: 1993-2022 Robert M Supnik
// SPDX-License-Identifier: MIT

#ifndef SCP_BREAKPOINT_H_
#define SCP_BREAKPOINT_H_

/* Breakpoint status shared with simulators and the SCP command layer. */
#define SIM_BRK_INILNT 4096
#define SIM_BRK_ALLTYP 0xFFFFFFFB

extern uint32 sim_brk_types;
extern uint32 sim_brk_dflt;
extern uint32 sim_brk_summ;
extern uint32 sim_brk_match_type;
extern t_addr sim_brk_match_addr;
extern BRKTYPTAB *sim_brk_type_desc;

/* Initialize the global breakpoint tables. */
t_stat sim_brk_init(void);

/* Set, clear, and display breakpoint definitions. */
t_stat sim_brk_set(t_addr loc, int32 sw, int32 ncnt, CONST char *act);
t_stat sim_brk_clr(t_addr loc, int32 sw);
t_stat sim_brk_clrall(int32 sw);
t_stat sim_brk_show(FILE *st, t_addr loc, int32 sw);
t_stat sim_brk_showall(FILE *st, int32 sw);

/* Find and test breakpoint table entries. */
BRKTAB *sim_brk_fnd(t_addr loc);
uint32 sim_brk_test(t_addr bloc, uint32 btyp);

/* Clear breakpoint-match timestamps for one space or all spaces. */
void sim_brk_clrspc(uint32 spc, uint32 btyp);
void sim_brk_npc(uint32 cnt);

/* Manage pending breakpoint actions. */
const char *sim_brk_getact(char *buf, int32 size);
char *sim_brk_clract(void);
void sim_brk_setact(const char *action);
char *sim_brk_replace_act(char *new_action);
const char *sim_brk_message(void);

#endif
