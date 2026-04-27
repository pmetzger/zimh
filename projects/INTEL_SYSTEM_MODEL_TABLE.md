# Intel System Model Table Cleanup

## Problem

`simulators/Intel-Systems/common/sys.c` stores per-device configuration
values in `SYS_DEV.val`, with `SYS_DEV.num` describing how many device
instances exist and `SYS_DEV.args` describing how many arguments each
instance consumes.

The current users index `val` as `val[j]`, `val[j + 1]`, and
`val[j + 2]` while iterating over instance `j`. That is safe for the
current table because all two-argument rows have `num == 1`, but it would
overlap values if a future two-argument row had multiple instances.

For example, a hypothetical row with `num == 2`, `args == 2`, and values
`{ A, B, C, D }` would currently use `(A, B)` for instance 0 and `(B, C)`
for instance 1, rather than `(A, B)` and `(C, D)`.

## Proposed Minimal Fix

Keep the existing table shape, but compute a per-instance base index:

```c
arg = j * models[model].devices[i].args;
```

Then use `val[arg]`, `val[arg + 1]`, and `val[arg + 2]` in:

- `sys_cfg`
- `sys_set_model`
- `sys_show_model`

This should be a small change and should not change current behavior,
because the current table does not use multi-instance rows with more than
one argument per instance.

## Testing

Run the Intel-Systems simulator tests after the change:

```sh
ctest --test-dir build/debug-warnings -L Intel-Systems --parallel \
    --output-on-failure
```

If practical, add a focused unit test or diagnostic helper around the
table walking logic before changing it. The table walker is simple, but
the existing structure makes accidental off-by-one mistakes easy.
