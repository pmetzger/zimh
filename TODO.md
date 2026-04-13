# TODO Items

## Documentation Conversion Methodology

The next major documentation task is to replace the legacy
Microsoft Word material under `docs/legacy-word/` with carefully
proofread Markdown in the main repository.

This work should use the existing `simh-docs/` repository as the
starting point. That repository already contains:

- Pandoc-generated Markdown drafts for the Word documents
- a set of documents believed to have already been hand-fixed into
  reasonable shape
- exported image/asset directories for documents that needed them

The general rule should be:

- proofread and adopt the `simh-docs/` material
- for documents that are still rough, continue from the existing
  Pandoc output rather than starting over unless there is a specific
  reason to do so
- use the already-believed-correct `simh-docs` conversions as the
  pattern for how future cleaned-up conversions should look

This work should proceed in these stages:

1. Audit every existing conversion from `simh-docs` before adopting it.
   No Markdown file or asset directory from `simh-docs` should be
   copied into this repository without a careful proofread against the
   original Word source.

2. Proofread the Pandoc conversions in a disciplined way.
   For each document that has a Markdown counterpart:
   - compare document structure, headings, tables, lists, footnotes,
     code blocks, and emphasized text against the Word original
   - repair Pandoc damage, formatting loss, bad wrapping, broken lists,
     and malformed tables
   - preserve and adopt any needed exported image assets from
     `simh-docs`
   - normalize the result to repository conventions such as line width,
     headings, links, file placement, and the style already established
     by the believed-correct `simh-docs` conversions
   - Editorial improvements in the believed-correct existing `simh-docs`
     conversions should be performed on the uncorrected Pandoc
     conversions, including but not limited to:
     - replacing duplicated inline license text with a stable license
       reference where that cleanup was already done correctly
     - converting Word file lists into readable Markdown tables or
       lists
     - promoting titles and section headings into normal Markdown
       heading structure instead of preserving Word visual formatting
     - retaining intentional hand-edited readability improvements even
       when they are not literal reproductions of the Word source, so
       long as meaning is preserved
     - correcting obvious source-document typos when doing so improves
       clarity without changing meaning
   - mark the tracker only after that proofreading pass is complete

3. Replace the Word documents only after the Markdown replacement is
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

4. Convert the remaining Word files one at a time.
   For documents that are still only raw Pandoc conversions:
   - begin from the existing `simh-docs` Pandoc output
   - hand-clean the draft heavily, following the structure and style of
     the already-believed-correct `simh-docs` conversions
   - proofread the result against the original Word file
   - update the tracker with problems found and resolved

5. Keep commits narrow and reviewable.
   A sensible commit pattern is:
   - one commit for tracker or process updates
   - one commit per document or small related document set
   - no large bulk-import commit of dozens of partially proofread files

6. Treat images, diagrams, and tables as first-class conversion work.
   Where Word documents contain embedded figures or tables, the tracker
   should note whether the existing `simh-docs` assets are sufficient or
   whether extraction, recreation, or manual cleanup is still needed
   before the document can be considered finished.


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
