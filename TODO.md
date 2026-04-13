# TODO Items

- Convert the legacy Word-format material under `docs/legacy-word/`
  into modern text-based formats.
- Decide whether `src/components/display/` should remain in-tree or be
  treated as third-party code and moved under `third_party/`.
- Decide the long-term status of the top-level `Makefile`:
  continue supporting it as a first-class build path, or eventually
  retire it in favor of CMake.
- Normalize repository naming and casing policies more broadly once the new
  directory hierarchy is stable.

## Notes

- The current top-level build-output directories such as `BIN/` and
  local out-of-tree build directories remain expected generated
  artifacts rather than pending layout work.
