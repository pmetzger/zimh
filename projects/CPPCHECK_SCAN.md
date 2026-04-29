# Cppcheck Scan

Run cppcheck across the entire codebase through the CMake `cppcheck` target
and triage the findings.

## Work

- Configure with `-DENABLE_CPPCHECK=ON` and the intended `ZIMH_C_STANDARD`.
- Run `cmake --build <build-dir> --target cppcheck`.
- Capture and group findings by subsystem.
- Fix clear correctness issues with focused tests first.
- Suppress or document false positives only after inspection.

## Acceptance Criteria

- A full-tree cppcheck run completes from the CMake target.
- Findings are either fixed, documented as follow-up projects, or explicitly
  suppressed with a reason.
