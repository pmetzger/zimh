# 3B2 Test Artifacts

These 3B2 binary artifacts are used by integration tests. They are
committed so the tests can run reproducibly.

## `3B2_Maintenance_Utilities_4-0.img`

- Source URL:
  `https://archives.loomcom.com/3b2/software/Maintenance_Utilities/3B2_Maintenance_Utilities_4-0.img`
- Source page:
  `https://archives.loomcom.com/3b2/software/Maintenance_Utilities/`
- Retrieved: 2026-04-26
- Size: 737280 bytes
- SHA-256:
  `4b31b05033f8d38a7235fe5221dc25a8b55d3fb85e4ae7effd5fa1804f06948e`
- Notes: 720 KB sector-by-sector diskette image for AT&T 3B2
  Maintenance Utilities 4.0.

## `3b2_400_maint_nvram.bin`

- Source: repository-generated test state.
- Size: 4096 bytes
- SHA-256:
  `2f5f099ad9370b6ca6118e6a74adf490385bb18277ca3af309f775eb030e971c`
- Notes: initialized 3B2/400 NVRAM state for the Maintenance Utilities
  smoke test. Tests must copy this file to a disposable output path
  before attaching it, because the NVRAM device does not support
  read-only attach.
