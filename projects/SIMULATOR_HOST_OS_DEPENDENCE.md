# Simulator Host-OS Dependence

Goal: move host-operating-system dependent code out of `simulators/`
and concentrate it in shared runtime/components layers or clearly
separated support code.

This is not a claim that every conditional in `simulators/` is bad.
Simulator-family and model-selection conditionals are normal. The target
here is code that depends on the host operating system, host GUI stack,
host timing APIs, or host-specific integration details.

## Current Inventory

### 1. Simulator-local display and video front ends

These are the clearest candidates for extraction, because the host
dependence is really about UI glue rather than machine emulation.

- `simulators/PDP1/pdp1_cpu.c`
- `simulators/PDP1/pdp1_dpy.c`
- `simulators/PDP1/pdp1_sys.c`
- `simulators/PDP8/pdp8_dpy.c`
- `simulators/PDP8/pdp8_sys.c`
- `simulators/PDP11/pdp11_daz.c`
- `simulators/PDP11/pdp11_ng.c`
- `simulators/PDP11/pdp11_stddev.c`
- `simulators/PDP11/pdp11_sys.c`
- `simulators/PDP11/pdp11_tv.c`
- `simulators/PDP11/pdp11_vt.c`
- `simulators/PDP10/ka10_stk.c`
- `simulators/PDP10/kx10_defs.h`
- `simulators/PDP10/kx10_sys.c`
- `simulators/PDP18B/pdp18b_defs.h`
- `simulators/TX-0/tx0_cpu.c`
- `simulators/TX-0/tx0_dpy.c`
- `simulators/TX-0/tx0_sys.c`
- `simulators/imlac/imlac_crt.c`
- `simulators/imlac/imlac_kbd.c`
- `simulators/linc/linc_crt.c`
- `simulators/linc/linc_kbd.c`
- `simulators/tt2500/tt2500_cpu.c`
- `simulators/tt2500/tt2500_crt.c`
- `simulators/tt2500/tt2500_key.c`
- `simulators/tt2500/tt2500_tv.c`
- `simulators/VAX/vax410_sysdev.c`
- `simulators/VAX/vax420_sysdev.c`
- `simulators/VAX/vax43_sysdev.c`
- `simulators/VAX/vax440_sysdev.c`
- `simulators/VAX/vax610_sysdev.c`
- `simulators/VAX/vax610_syslist.c`
- `simulators/VAX/vax630_sysdev.c`
- `simulators/VAX/vax630_syslist.c`
- `simulators/VAX/vax_sysdev.c`
- `simulators/VAX/vax_syslist.c`
- `simulators/BESM6/besm6_panel.c`
- `simulators/AltairZ80/sol20.c`

These mostly depend on:

- `USE_DISPLAY`
- `USE_SIM_VIDEO`
- `HAVE_LIBSDL`

Desired direction:

- keep simulator-visible device models in `simulators/`
- move host display/video integration behind reusable runtime/component
  interfaces
- leave simulator files with only calls into that shared layer

### 2. Simulator-local host timing or sleep calls

These are smaller than the display cases, but still represent host API
dependence inside simulator code.

- `simulators/SEL32/sel32_ipu.c`
  - `nanosleep()` / `Sleep()`
- `simulators/PDP10/ka10_pipanel.c`
  - `nanosleep()`
- `simulators/AltairZ80/altairz80_sio.c`
  - Windows and sleep-path handling
- `simulators/AltairZ80/s100_fif.c`
  - commented `Sleep()` usage

Desired direction:

- use shared runtime time/sleep wrappers
- keep simulator code independent of host timing primitives

### 3. Simulator-local host networking branches

These should be reviewed to see whether the host-specific work belongs
in shared runtime networking support instead.

- `simulators/3B2/3b2_ni.c`
- `simulators/PDP11/pdp11_xq.c`
- `simulators/PDP11/pdp11_xu.c`

Desired direction:

- move host NIC/driver distinctions into shared networking/runtime code
- keep simulator device logic focused on emulated controller behavior

### 4. IBM 1130 host-integration code

The IBM 1130 tree has the heaviest simulator-local host dependence.
Some of this is GUI support, and some appears to be direct integration
with host devices or Windows-specific facilities.

- `simulators/Ibm1130/ibm1130_cpu.c`
- `simulators/Ibm1130/ibm1130_cr.c`
- `simulators/Ibm1130/ibm1130_gdu.c`
- `simulators/Ibm1130/ibm1130_gui.c`
- `simulators/Ibm1130/ibm1130_prt.c`
- `simulators/Ibm1130/ibm1130_sys.c`
- `simulators/Ibm1130/ibm1130_defs.h`
- `simulators/Ibm1130/ibm1130.rc`
- `simulators/Ibm1130/CMakeLists.txt`

Notable examples:

- direct Windows API device access in `ibm1130_cr.c`
- Windows GUI/resource handling in `ibm1130_gui.c` and `ibm1130.rc`
- Windows-only build wiring in `Ibm1130/CMakeLists.txt`

Desired direction:

- split GUI support from simulator logic
- isolate physical-device support behind shared host-service layers
- make the simulator core buildable without Windows-specific code mixed
  through the emulation files

### 5. Minor build or parser portability in simulator-local files

These are lower priority than the UI/network/timing cases above, but
they are still host-awareness living under `simulators/`.

- `simulators/SAGE/m68k_parse.y`
  - `_WIN32` conditional
- `simulators/SAGE/chip_defs.h`
  - `UNIX_PLATFORM`
- `simulators/SSEM/ssem_cpu.c`
  - `UNIX_PLATFORM`
- `simulators/I650/CMakeLists.txt`
  - Windows-specific linker stack option

Desired direction:

- keep platform build policy in CMake helpers or runtime support
- keep simulator-local files focused on machine behavior

## Deferred / Separate From Main Cleanup

These are host-dependent, but they should not drive the first cleanup
phase because they are either utilities or bundled sidecar code.

### IBM 1130 utilities

- `simulators/Ibm1130/utils/*.c`
- `simulators/Ibm1130/utils/*.mak`

These matter, but they should likely be handled together with the
general IBM 1130 utility/build-path decision.

### Bundled AltairZ80 Musashi sidecar code

- `simulators/AltairZ80/m68k/**`

This subtree has plenty of platform/compiler conditionals, but much of
it is third-party or sidecar code rather than main simulator logic.

## Proposed Order

1. Extract shared sleep/time use out of the small simulator-local sites.

2. Tackle simulator display/video glue as a category.

3. Review simulator-local networking branches and move host distinctions
   into shared runtime support where possible.

4. Do a focused IBM 1130 host-integration split.

5. Clean up the remaining simulator-local build/parser portability
   leftovers.

## Definition of Done

- maintained simulator emulation code in `simulators/` no longer makes
  direct host-OS choices except where a simulator genuinely models a
  host-facing device that still needs a small adapter
- host GUI, video, networking, timing, and physical-device integration
  live in shared support layers or clearly separated support modules
- simulator-local CMake files do not contain avoidable Windows/Unix
  policy that belongs in common build helpers
