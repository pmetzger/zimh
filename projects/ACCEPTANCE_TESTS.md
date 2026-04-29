# Acceptance Tests

Before cutting releases, or when we are worried about large changes, we
should have an optional acceptance test suite with much broader coverage than
normal developer builds.

The suite should be easy to run explicitly, but it must not be part
of every routine local build. Its purpose is to catch broad build,
configuration, compiler, and integration problems before they reach users.

Acceptance tests may include some extensive tests of machine
simulation correctness that might take hours or even days to run.

## C Standard Matrix

One acceptance test should verify that basic zimh behavior works across the
C standard modes we care about for compatibility and release validation.

For each of these compiler settings:

- C99
- C11
- C17
- C23

the acceptance test should:

- configure a clean build tree
- build zimh
- run the unit tests
- run the integration tests

This gives us confidence that routine code changes and build-system changes
have not accidentally tied zimh to one compiler standard mode.
