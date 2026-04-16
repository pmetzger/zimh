# `sim_fio.c` bug report: `sim_fwrite()` advances the source pointer
# incorrectly across multiple flip buffers

## Summary

`sim_fwrite()` in `src/runtime/sim_fio.c` has a real bug in the
big-endian/swapped write path. When a write spans more than one internal
flip buffer, the function advances its source pointer by
`size * count` on every chunk instead of by `size * c`, where `c` is the
number of elements actually written in that chunk.

This causes later chunks to read from the wrong part of the caller's
buffer, which corrupts the file contents.

## Where the bug is

In `src/runtime/sim_fio.c`, inside `sim_fwrite()`:

```c
for (i = (int32)nbuf; i > 0; i--) {                     /* loop on buffers */
    c = (i == 1)? lcnt: nelem;
    sim_buf_copy_swapped (sim_flip, sptr, size, c);
    sptr = sptr + size * count;
    c = fwrite (sim_flip, size, c, fptr);
```

The source pointer update should use `c`, not `count`.

## Why this is wrong

`sim_fwrite()` may break the caller's write into multiple pieces:

- `nelem` is the number of elements that fit in the flip buffer
- `nbuf` is the number of chunks to write
- `c` is the number of elements in the current chunk

After writing one chunk, the source pointer must advance by the size of
that chunk:

```c
sptr = sptr + size * c;
```

But the current code advances by the size of the entire original request
every time:

```c
sptr = sptr + size * count;
```

That is only correct if there is exactly one chunk.

## Minimal patch

```diff
diff --git a/src/runtime/sim_fio.c b/src/runtime/sim_fio.c
@@
     c = (i == 1)? lcnt: nelem;
     sim_buf_copy_swapped (sim_flip, sptr, size, c);
-    sptr = sptr + size * count;
+    sptr = sptr + size * c;
     c = fwrite (sim_flip, size, c, fptr);
```

## How this was found

This was exposed while deepening host-side unit coverage for
`src/runtime/sim_fio.c`.

A new test forces `sim_fwrite()` to use more than one flip buffer by
writing more than:

```c
FLIP_SIZE / sizeof(uint16_t)
```

elements with `sim_end = FALSE`, then reads the data back with
`sim_fread()` and checks that the logical values round-trip unchanged.

Before the patch, the test failed once the second chunk was reached.

## Reproducer shape

In outline:

1. Force the swapped path:
   - `sim_end = FALSE`
2. Write more than one flip buffer of `uint16_t` values
3. Read the file back with `sim_fread()`
4. Compare the result against the original buffer

Before the fix, values in the later portion of the file came from the
wrong source region.

## Impact

This affects endian-swapped writes that are large enough to require more
than one internal flip-buffer chunk.

Small writes do not expose the problem, which is why it can remain hidden
unless specifically tested.
