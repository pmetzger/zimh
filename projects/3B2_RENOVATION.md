# 3B2 Renovation

## Goal

Turn the 3B2 simulator family from a mostly placeholder-documented,
lightly-tested target into a simulator with credible documentation,
committed provenance for required ROM/software artifacts, and real
integration coverage using bootable 3B2 software.

This project should stay practical: prefer small, reviewable improvements
to the existing simulator, tests, and documentation over large rewrites.

## Primary Sources

Use Loomcom as the preferred source for 3B2 material:

- 3B2 archive index: `https://loomcom.com/3b2/`
- Emulator documentation: `https://loomcom.com/3b2/emulator/`
- Maintenance Utilities archive:
  `https://archives.loomcom.com/3b2/software/Maintenance_Utilities/`

## Documentation

`docs/simulators/3B2/3b2.md` now has initial user documentation based on
the Loomcom emulator documentation and repository-local source facts. The
next documentation work is to keep tightening that page as behavior is
proved or changed. It should continue to document:

- supported simulator targets: `3b2-400` and `3b2-700`;
- model coverage and important differences between the 3B2/400 and
  3B2/700 simulator paths;
- memory size options;
- simulated devices and their SIMH names;
- boot workflow, including ROM expectations and media attachment;
- integrated floppy behavior and diskette image format;
- hard disk, SCSI, Ethernet, serial, cartridge tape, and NVRAM notes;
- known limitations and areas where the emulator is incomplete;
- where external software, ROMs, manuals, and disk images came from.

Do not copy large blocks of third-party documentation verbatim. Summarize
and cite the source URLs.

## Future Artifact Rules

For any future committed binary artifact, keep provenance near the
artifact, do not fetch it from the network during automated tests, and
do not let tests modify original artifact bytes directly.

## Maintenance Utilities

Keep Maintenance Utilities automation scoped to `3b2-400` unless we find
and manually prove a SCSI-series maintenance disk image.

`https://archives.loomcom.com/3b2/software/Maintenance_Utilities/`

The archive says one disk supports the 300/310/400 series and another
supports the SCSI 500/600/700/1000 series. The visible listing currently
exposes a single `Maintenance Utilities 4.0` image, and the repository
test has only proved that image on `3b2-400`. Do not treat this test as
3B2/700 coverage unless a SCSI-series maintenance disk is separately
identified and manually proved.

If DGMON has useful but slow diagnostics, document a separate manual or
long-running test plan rather than slowing the default integration suite.
Do not run hard disk or floppy formatting utilities in CI.

## 3B2/700 Acceptance Coverage

The current 3B2/700 proof is manual. Loomcom provides a prebuilt 700 UNIX
image and state files:

- `https://archives.loomcom.com/3b2/emulator/700/sd630.img.gz`
- `https://archives.loomcom.com/3b2/emulator/700/nvram.bin`
- `https://archives.loomcom.com/3b2/emulator/700/tod.bin`

The manual test attached the 630 MB SCSI disk at ID 1, enabled the
Wangtek tape device at ID 2, enabled the PORTS device, attached the
downloaded NVRAM and TOD files, and booted the system to the UNIX console
login prompt without an explicit `LOAD -r` command. Because the UNIX
image is not being committed to the repository, keep this as a documented
manual acceptance test unless we later choose a different artifact policy
or build an optional local artifact test.

## Open Questions

- Is the SCSI-series Maintenance Utilities disk available under another
  Loomcom/archive path, or is only the current 300/310/400-proven image
  readily available?
- Should 3B2/700 real-software acceptance remain manual, or should we
  define an optional local-artifact test that uses external Loomcom UNIX
  media without committing that media to the repository?
- Are there small, non-destructive DGMON commands worth adding to the
  default 3B2/400 smoke test?
