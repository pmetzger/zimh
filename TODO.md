# TODO Items

## Documentation Conversion Methodology

The next major documentation task is to replace the legacy Microsoft
Word material under `docs/legacy-word/` with carefully proofread
Markdown in the main repository.

The conversion tracker is:
`docs/legacy-word/doc-conversion-tracker.md`

Supporting plain-text exports of Word documents are under
`tmp/word-text/`.

The working base for this effort is the existing `simh-docs/`
repository. It already contains:

- Pandoc-generated Markdown drafts for the Word documents
    - Some of the documents have already been hand or machine corrected
- exported image and asset directories for documents that need them

The default rule is to proofread and adopt the `simh-docs/` material
rather than reconvert from scratch. If a document is still rough,
continue from the existing Pandoc output unless there is a specific
reason to start over. Use the already proofread `simh-docs`
conversions as the style model for future cleaned-up conversions.

### Per-Document Workflow

The remaining `simh-docs` drafts should be treated as having already
received the initial normalization pass for their leading structure.
Do not spend time redoing that pass unless a specific document shows a
clear problem.

Each remaining unadopted document should be reviewed section by
section, not only as a whole-file cleanup pass.

For each document:

1. Read and compare the document one section at a time.
   - use the `simh-docs` draft as the working copy
   - compare each section against the Word source or supporting text
     export before moving on
   - apply clearly mechanical or obvious fixes immediately
   - do not defer straightforward repairs such as formatting damage,
     broken tables, corrupted punctuation, obvious command-formatting
     problems, or clearly wrong technical identifiers once verified

2. Maintain a per-document review tracker for judgment calls.
   - when a proposed change goes beyond the mechanical or obvious,
     record it in a tracker file for human review rather than applying
     it immediately
   - the tracker should record these as clear yes-or-no items in the
     style used for the Altair review
   - each item should be short, concrete, and tied to a specific
     section or local issue
   - once reviewed by a human, apply approved items and remove or mark
     resolved entries so the tracker remains usable

3. Distinguish mechanical fixes from judgment calls consistently.
   - mechanical or obvious fixes should be applied directly
   - judgment-call edits should go into the tracker first
   - if there is any real risk of changing technical meaning, treat
     the change as a judgment call

4. While reviewing, watch for repeated issue patterns that may affect
   multiple remaining documents.
   - examples include command names left in quotation marks instead of
     backquotes, spurious backslashes inside backquoted text, repeated
     table-conversion defects, or recurring syntax-formatting damage
   - once a pattern has been verified as a real cleanup category,
     treat it as a tracked cross-document review item for the
     remaining unadopted documents

5. Create beads issues for verified cross-document cleanup patterns.
   - once a recurring issue pattern has been verified as real, create
     a beads issue for each remaining document and each section that
     should be checked for that pattern
   - each such issue should cover both checking whether the pattern is
     present in that document section and fixing it if it is present
   - issue descriptions should identify the pattern, the document, and
     the affected section or section range
   - use these issues to ensure repeated cleanup work is tracked
     explicitly rather than being rediscovered ad hoc

6. Decide whether the document is structurally ready for adoption after
   the section-by-section pass.
   - if it is still visibly raw Pandoc output, do a dedicated
     structure-cleanup pass first
   - if it is already close, proceed directly to detailed
     proofreading
   - do not adopt a document that still contains:
     - fake page-number TOC entries
     - malformed tables or lists
     - broken code or syntax formatting
     - obvious conversion corruption
     - unverified technical names, commands, registers, device types,
       or model numbers
     - unresolved judgment-call items that still need human review
     - unexplained substantive differences from the Word original

7. Reread the cleaned Markdown as Markdown before adoption.
   - do not rely only on the diff
   - check the front matter, TOC, tables, and example blocks
   - confirm the tracker entry accurately describes what changed
   - confirm any verified cross-document issue patterns have either
     been addressed in this document or have explicit beads coverage

8. Adopt the document into the main repository only after the
   Markdown version can stand on its own as the maintained copy.
   - finish the cleaned `simh-docs` version first
   - place it in its final location in this repository
   - update references and indexes as needed
   - update the tracker destination and notes
   - keep the working `simh-docs` copy in sync with the adopted
     version unless there is a specific reason not to
   - only then remove or retire the Word file from active use

### Adoption Policy

A document is considered adopted once the cleaned Markdown has been
placed in its final repository location and the conversion tracker
points to that maintained copy.

Default placement policy:

- simulator manuals should go under `docs/simulators/`
- SIMH-wide manuals should go under `docs/project/`
- standalone articles should go under `docs/articles/`
- historical standalone pieces, if any, should go under
  `docs/history/`

Adopted filenames should usually drop `_doc` unless that suffix is
actually useful. Prefer short descriptive names and update the tracker
when the final adopted name differs from the Word filename or the
`simh-docs` draft name.

The tracker should also note whether the existing `simh-docs` image
and asset exports were sufficient or whether extraction, recreation,
or manual cleanup was needed before the document could be considered
finished.

### Review-Tracker Policy

For documents still under section-by-section review, keep a companion
tracker that records non-mechanical proposed changes requiring human
judgment.

That tracker should:

- be organized by document and section
- use short yes-or-no review items
- separate unapplied judgment calls from already resolved items
- make it easy to apply approved edits without re-reading the entire
  document to rediscover them

When a reviewed issue turns out to represent a broader recurring
defect across the remaining unadopted documents, create the
corresponding beads issues at that point. Those issues should cover
checking for the pattern in the assigned document section and fixing
it where present.

### Commit and Review Guidance

Keep commits narrow and reviewable.

- one commit for tracker or process updates
- one commit per document or small related document set
- no large bulk-import commit of dozens of partially proofread files


## Other Pending Items

- Decide whether `src/components/display/` should remain in-tree or be
  treated as third-party code and moved under `third_party/`.
- Decide the long-term status of the top-level `Makefile`:
  continue supporting it as a first-class build path, or eventually
  retire it in favor of CMake.
- Normalize repository naming and casing policies more broadly once the
  new directory hierarchy is stable.
- Purge obsolete host operating system support references and code
  paths. Search for and remove or rewrite references to all of the
  following host platforms and closely related labels so nothing is
  missed:
  - `VMS`
  - `OpenVMS`
  - `VAX VMS`
  - `Alpha VMS`
  - `IA64 VMS`
  - `Alpha UNIX`
  - `Digital UNIX`
  - `Tru64 UNIX`
  - `Solaris`
  - `SunOS`
  - `OS/2`
  - `HP-UX`
  - `AIX`
  - `Cygwin`
  - obsolete Windows target/version names:
    `Windows 9x`, `Windows NT`, `Windows 2000`, `Windows XP`,
    `Win7`, `Win8`
  - obsolete Windows build/toolchain labels that may need review while
    host support is narrowed:
    `Visual C++`, `Visual C++ .NET`, `MinGW`, `MinGW-w64`

## Notes

- The `simh-docs` repository reports a mix of proofread conversions and
  near-raw Pandoc output. It should be treated as the working base for
  adoption, with the proofread files serving as the style model for the
  remaining cleanup work.
- The current top-level build-output directories such as `BIN/` and
  local out-of-tree build directories remain expected generated
  artifacts rather than pending layout work.
