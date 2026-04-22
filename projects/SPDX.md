# SPDX

Policy for new code:

- use SPDX headers in new ZIMH-owned source files as they are added
- convert older SIMH-owned source files to SPDX as we touch them
  - use `MIT` when the original header is MIT-like
  - use `X11` when the original header includes the advertising clause
- do not rewrite imported third-party source just to convert license text

Current status:

- `src/core` and `src/runtime` have been converted to SPDX headers
- future new ZIMH-owned source files should use SPDX from the start
