# Stub Simulator Skeleton

This directory contains a minimal SIMH simulator skeleton. It is useful as a
starting point for a new simulator or for understanding the smallest set of
pieces needed to hook a machine into the SIMH framework.

The main files are:

- `stub_cpu.c`
  Defines the CPU device, memory examine/deposit handlers, and the
  instruction-execution loop entry point.
- `stub_sys.c`
  Defines the simulator-global state such as `sim_name`, `sim_devices`,
  stop messages, and loader/symbol hooks.
- `stub_defs.h`
  Shares the minimal declarations used by the stub sources.

## What You Are Expected To Fill In

The stub is intentionally incomplete.

- `sim_instr`
  Add instruction fetch, decode, and execution logic here.
- `sim_load`
  Load an input file into the simulator's memory or other state.
- `cpu_reg`
  Describe the machine state that should be visible through SIMH.
- `parse_sym` and `fprint_sym`
  Implement assembly and disassembly support for `DEPOSIT` and
  `EXAMINE -M`.
- `sim_devices`
  Add any peripheral `DEVICE *` entries for the machine.
- `build_dev_tab`
  Initialize any runtime data structures needed by those devices.

## Runtime Model

The stub already demonstrates the normal SIMH event loop shape:

- `sim_instr` checks pending events
- `sim_process_event()` runs when the event queue reaches the current time
- breakpoint handling goes through the normal SIMH breakpoint machinery

If the simulator needs asynchronous or delayed device behavior, use the normal
SIMH event facilities:

- `sim_activate` schedules a future event
- the unit service (`svc`) routine runs when that event fires

## Related Reading

For broader simulator-writing guidance, also see:

- `docs/developers/writing_a_simulator.md`
