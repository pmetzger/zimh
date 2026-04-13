# TODO Items

## Documentation Conversion Methodology

The next major documentation task should be to replace the legacy
Microsoft Word material under `docs/legacy-word/doc/` with carefully
proofread Markdown in the main repository.

This work should proceed in these stages:

1. Create and maintain a conversion tracker file, likely
   `docs/project/doc-conversion-tracker.md`, with one row for every
   legacy Word document.
   Each row should record at least:
   - source Word filename
   - proposed Markdown destination
   - whether a `simh-docs` conversion already exists
   - status: not started, imported, proofread, replaced, done
   - notes on figures, tables, formatting damage, or open questions

2. Inventory and map the current corpus before changing anything.
   The tracker should cover all files currently under
   `docs/legacy-word/doc/`, including special-name cases such as
   `Summary of IMP IO Device Codes.doc`.
   Converted manuals should live under `docs/`, not under
   `simulators/`.

3. Audit every existing conversion from `simh-docs` before adopting it.
   No Markdown file from `simh-docs` should be copied into this
   repository without a careful proofread against the original Word
   source.

4. Proofread existing conversions in a disciplined way.
   For each document that already has a Markdown counterpart:
   - compare document structure, headings, tables, lists, footnotes,
     code blocks, and emphasized text against the Word original
   - repair Pandoc damage, formatting loss, bad wrapping, broken lists,
     and malformed tables
   - normalize the result to repository conventions such as line width,
     headings, links, and file placement
   - mark the tracker only after that proofreading pass is complete

5. Replace the Word documents only after the Markdown replacement is
   good enough to stand on its own.
   The practical sequence for each document should be:
   - prepare or repair the Markdown conversion
   - proofread it against the Word original
   - place it in its final location in this repository
   - update references and indexes as needed
   - only then remove or retire the Word file from active use

   As a default placement policy:
   - simulator manuals should go under `docs/simulators/<family>/`
   - SIMH-wide manuals should go under `docs/project/`
   - articles or historical standalone pieces should go under an
     appropriate `docs/` subdirectory rather than inside a simulator
     tree

6. Convert the remaining Word files one at a time.
   For documents that only exist as raw Word sources:
   - generate an initial Markdown draft with the chosen conversion tool
   - hand-clean the draft heavily
   - proofread the result against the original Word file
   - update the tracker with problems found and resolved

7. Keep commits narrow and reviewable.
   A sensible commit pattern is:
   - one commit for tracker or process updates
   - one commit per document or small related document set
   - no large bulk-import commit of dozens of partially proofread files

8. Treat images, diagrams, and tables as first-class conversion work.
   Where Word documents contain embedded figures or tables, the tracker
   should note whether extraction, recreation, or manual cleanup is
   needed before the document can be considered finished.

9. Do not begin mass conversion until the methodology and toolchain are
   explicitly approved.

## Proposed Tooling For Approval

The currently visible local tooling is not sufficient for the full
conversion effort. Before starting the actual document work, it would
be reasonable to install:

- `pandoc`
  Used to produce initial Markdown drafts from `.doc` and `.docx`
  material.
- `libreoffice`
  Needed to open and visually inspect the original Word documents,
  especially old `.doc` files and formatting-sensitive manuals.
- `antiword` or an equivalent legacy `.doc` extraction tool
  Useful as a secondary textual check for older Word files when Pandoc
  or LibreOffice import quality is questionable.

Optional but likely helpful:

- a spell-checking tool such as `aspell` or `hunspell`
- a diff-friendly Markdown formatter if we later decide we want one

## Other Pending Items

- Decide whether `src/components/display/` should remain in-tree or be
  treated as third-party code and moved under `third_party/`.
- Decide the long-term status of the top-level `Makefile`:
  continue supporting it as a first-class build path, or eventually
  retire it in favor of CMake.
- Normalize repository naming and casing policies more broadly once the
  new directory hierarchy is stable.

## Notes

- The `simh-docs` repository reports a mix of proofread conversions and
  near-raw Pandoc output. It should be treated as source material for
  adoption, not as already-finished documentation.
- The current top-level build-output directories such as `BIN/` and
  local out-of-tree build directories remain expected generated
  artifacts rather than pending layout work.
