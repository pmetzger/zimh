# C++ Guard Cleanup

## Goal

Remove the pervasive `#ifdef __cplusplus` / `extern "C"` guards from the
tree unless there is a real and current need for them.

The project is written in C, and every conditional compilation block
adds cost:

- more header clutter
- more surface area for mistakes
- more visual noise when reading APIs
- more places where future edits have to preserve dead scaffolding

## Current State

The tree contains many `__cplusplus` guards in public headers and a few
source files.

There does not currently appear to be in-tree C++ source using these
headers, but the guards are an established convention across the code
base.

That means this should be treated as a deliberate cleanup project, not a
casual line-by-line deletion.

## Plan

1. Audit the actual C++ usage story.
   - confirm whether any build target, external interface, or supported
     consumer genuinely requires C linkage guards

2. If there is no real requirement, remove the guards in stages:
   - start with newer or narrower headers
   - then move through the shared runtime/core headers

3. If a small number of headers truly need C linkage guards, document
   those as exceptions rather than keeping the pattern everywhere.

## Definition Of Done

- `__cplusplus` guards are removed from headers that do not need them
- any remaining guards are documented exceptions with a real consumer
- new headers do not add the pattern by default
