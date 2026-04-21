**Goal**

Track the next concrete places where the new `sim_dynstr` utility should
replace local grow-string logic.

**First-tier candidates**

These have obvious ad hoc growth code or a local grow-string helper and
should be the first places to evaluate for direct `sim_dynstr` adoption.

- `src/runtime/sim_tmxr.c`
  - has a local `growstring()` helper at line 899
  - uses repeated `sprintf(growstring(...), ...)` assembly for mux and line
    attach strings
  - this is the clearest next candidate

- `src/core/scp.c`
  - has several repeated "formatted result didn't fit, grow buffer and retry"
    loops around:
    - `sim_printf`
    - `sim_messagef`
    - `_sim_vdebug`
    - other formatted-output helpers near lines 10671, 10748, 10840, 11051
  - may want either `sim_dynstr` directly or a small formatted-append helper
    built on top of it

- `src/runtime/sim_frontpanel.c`
  - has similar repeated formatted-buffer growth loops around lines 300,
    2598, and 2637
  - same likely treatment as `scp.c`

**Second-tier candidates**

These do not necessarily need `sim_dynstr` immediately, but they are using
manual string growth or append patterns that should be revisited.

- `src/core/scp_breakpoint.c`
  - grows `sim_brk_act_buf` with `realloc` and then appends via `strlcat`
  - worth checking whether a dynamic string object would simplify the action
    buffer handling

- `src/core/scp.c`
  - command-line assembly around line 2690 uses
    `sprintf(&cbuf[strlen(cbuf)], ...)`
  - may be fine as-is if the enclosing buffer contract is small and clear,
    but it should be examined

- `src/runtime/sim_ether.c`
  - several `sprintf(&buf[strlen(buf)], ...)` sequences in the BPF filter
    assembly path around lines 3841-3889
  - could be a dynstr caller if the buffer bounds are awkward

**Likely not dynstr work**

These showed up in the scan but are probably not good first `sim_dynstr`
targets.

- fixed-buffer `strlcat` accumulation where the buffer is intentionally small
  and bounded:
  - `src/core/scp_help_engine.c`
  - `simulators/SEL32/sel32_ec.c`
  - `simulators/PDP10/kx10_imp.c`
  - `simulators/PDP10/kl10_nia.c`
  - `simulators/PDQ-3/pdq3_fdc.c`

- one-off `realloc(strlen(...) + 1)` ownership changes rather than string
  building:
  - `simulators/SAGE/sage_cpu.c`
  - `src/core/scp.c` `sim_on_actions[...]`
  - `src/runtime/sim_ether.c` `dev->bpf_filter`

- generated or third-party-ish sources with lots of `sprintf(...strlen...)`
  patterns:
  - `simulators/AltairZ80/m68k/*`
  - `simulators/AltairZ80/m68k/example/*`

**Recommended order**

1. `src/runtime/sim_tmxr.c`
2. `src/core/scp.c` formatted-output helpers
3. `src/runtime/sim_frontpanel.c`
4. reevaluate smaller second-tier spots only after those are done
