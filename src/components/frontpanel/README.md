# Front Panel Sample

`frontpaneltest` is a sample program and manual diagnostic for the
`sim_frontpanel` API.

It is not itself a simulator. Instead, it links against:

- `src/runtime/sim_frontpanel.c`
- `src/runtime/sim_sock.c`

and uses the front-panel API to:

- start a simulator process
- connect to it through the front-panel interface
- exercise front-panel control operations
- display register state
- provide an interactive command loop for manual experimentation

In its current form, it is aimed at a VAX setup and expects to start and
control a `vax` simulator.

## What It Is Not

`frontpaneltest` is not part of the automated regression test suite.

That is intentional. The program behaves more like an interactive sample or
manual integration harness than a deterministic automated test.

Among other things, it:

- starts another simulator process
- creates a temporary configuration file
- may try to launch terminal or telnet helpers on some platforms
- drops into an interactive command loop

For now it should be treated as a manual tool. If we ever want automated
coverage here, the right direction is to split out a headless smoke test and
leave the interactive demo separate.

## Building It

From a configured build tree:

```sh
cmake --build build/release --target frontpaneltest
```

The resulting executable is written to:

- `build/release/bin/frontpaneltest`

## Related API Documentation

The public API used by this sample is documented in:

- `src/runtime/sim_frontpanel.h`
