## Build artifact purge plan

This note tracks old build-system artifacts still present in the tree
after the maintained build moved to CMake plus a thin compatibility
top-level `Makefile`.

The goal is to delete dead build files, not to remove still-needed build
paths by accident.

## Backlog

### 1. IBM 1130 utility build files

These still appear to be the only recorded build path for code that
exists in the tree, even though they are not part of the maintained
CMake build:

- `simulators/Ibm1130/utils/makefile`
- `simulators/Ibm1130/utils/*.mak`

Why they are not safe to purge yet:
- `simulators/Ibm1130/CMakeLists.txt` does not yet build the utilities
- current simulator docs still tell users to use these files
- removing them now would silently strand those tools

Required decision first:
- either migrate the IBM 1130 utilities into CMake
- or explicitly declare them unsupported and remove both the build files
  and the related documentation

### 2. Embedded/sidecar legacy build files

These are not used by the maintained simulator build, but may still be
useful for working on the embedded component or its examples directly:

- `simulators/AltairZ80/m68k/Makefile`
- `simulators/AltairZ80/m68k/example/Makefile`

Required decision first:
- keep them as component-local upstream build aids
- or decide that the CMake-integrated simulator build is the only
  supported path and remove them

### 3. Historical display build instructions

The display subtree still contains old local README text that documents
the standalone makefiles and batch wrappers already removed:

- `src/components/display/README`

That file should be cleaned up separately as historical documentation,
not rewritten opportunistically during artifact deletion.

### 4. Not purge candidates

These are old-looking in places, but still part of the current supported
build or compatibility story:

- top-level `Makefile`
- Windows support in `cmake/`
- current Visual Studio generator handling in CMake

## Proposed order

1. decide the fate of the IBM 1130 utility build files
2. decide the fate of the Altair/Musashi sidecar makefiles
3. clean up the historical display README text
4. clean up broader documentation references once the tree-level policy
   is settled

## Success criteria

- dead build artifacts are removed from the tree
- remaining non-CMake build files have an explicit reason to exist
- documentation no longer points at build paths we do not support
