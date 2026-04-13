# TODO Items

## Documentation Conversion Methodology

The next major documentation task is to replace the legacy
Microsoft Word material under `docs/legacy-word/doc/` with carefully
proofread Markdown in the main repository.

This work should use the existing `simh-docs/` repository as the
starting point. That repository already contains:

- Pandoc-generated Markdown drafts for most of the Word documents
- a set of documents believed to have already been hand-fixed into
  reasonable shape
- exported image/asset directories for documents that needed them

The general rule should be:

- adopt and proofread the existing `simh-docs/` material rather than
  regenerating work unnecessarily
- for documents that are still rough, continue from the existing
  Pandoc output rather than starting over unless there is a specific
  reason to do so
- use the already-believed-correct `simh-docs` conversions as the
  pattern for how future cleaned-up conversions should look

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
   No Markdown file or asset directory from `simh-docs` should be
   copied into this repository without a careful proofread against the
   original Word source.

4. Proofread existing conversions in a disciplined way.
   For each document that already has a Markdown counterpart:
   - compare document structure, headings, tables, lists, footnotes,
     code blocks, and emphasized text against the Word original
   - repair Pandoc damage, formatting loss, bad wrapping, broken lists,
     and malformed tables
   - preserve and adopt any needed exported image assets from
     `simh-docs`
   - normalize the result to repository conventions such as line width,
     headings, links, file placement, and the style already established
     by the believed-correct `simh-docs` conversions
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
   - simulator manuals should go under `docs/simulators/`
   - SIMH-wide manuals should go under `docs/project/`
   - standalone articles should go under `docs/articles/`
   - historical standalone pieces if any should go under `docs/history/`

6. Convert the remaining Word files one at a time.
   For documents that are still only raw Pandoc conversions:
   - begin from the existing `simh-docs` Pandoc output
   - hand-clean the draft heavily, following the structure and style of
     the already-believed-correct `simh-docs` conversions
   - proofread the result against the original Word file
   - update the tracker with problems found and resolved

7. Keep commits narrow and reviewable.
   A sensible commit pattern is:
   - one commit for tracker or process updates
   - one commit per document or small related document set
   - no large bulk-import commit of dozens of partially proofread files

8. Treat images, diagrams, and tables as first-class conversion work.
   Where Word documents contain embedded figures or tables, the tracker
   should note whether the existing `simh-docs` assets are sufficient or
   whether extraction, recreation, or manual cleanup is still needed
   before the document can be considered finished.

9. Do not begin mass conversion until the methodology and toolchain are
   explicitly approved.


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
  near-raw Pandoc output. It should be treated as the working base for
  adoption, with the proofread files serving as the style model for the
  remaining cleanup work.
- The current top-level build-output directories such as `BIN/` and
  local out-of-tree build directories remain expected generated
  artifacts rather than pending layout work.
