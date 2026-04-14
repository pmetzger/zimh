# TODO Items

## Documentation Conversion Methodology

The next major documentation task is to replace the legacy
Microsoft Word material under `docs/legacy-word/` with carefully
proofread Markdown in the main repository.

(Pre-converted versions of the word document into text using soffice
are located in `tmp/word-text/altairz80_doc.txt`.)

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

1. Start each document with an initial normalization pass in
   `simh-docs`.
   - clean up the front matter to the current standard
   - replace fake page-number TOCs with real linked TOCs
   - reflow ordinary prose toward the 80-column preference where that
     improves readability
   - make only those broad presentational fixes first
   - then let a human review that diff and, if desired, make an interim
     `simh-docs` commit before more detailed cleanup

2. After that initial pass, decide how rough the document still is.
   - if it is still visibly raw Pandoc output, do a dedicated
     structure-cleanup pass
   - if it is already close, proceed directly to detailed proofreading
   - a document is not ready for adoption if any of these remain:
     - fake page-number TOC entries
     - malformed tables or lists
     - broken code or syntax formatting
     - obvious conversion corruption
     - unverified technical names, commands, registers, device types,
       or model numbers
     - unexplained substantive differences from the Word original

3. Do a careful source-comparison pass against the Word original.
   - compare headings, structure, tables, lists, syntax blocks,
     examples, and emphasized text
   - preserve and adopt any needed exported image assets from
     `simh-docs`
   - repair Pandoc damage, formatting loss, bad wrapping, broken lists,
     and malformed tables
   - verify technical-looking identifiers against source code where
     practical:
     - commands
     - option names
     - register names
     - device names
     - model numbers
     - controller names
   - if the Word document is wrong, correct the Markdown and note that
     in the tracker or adoption summary

4. Then do restrained editorial cleanup.
   - preserve wording when it is historically intentional or
     technically specific
   - improve wording when it is clearly:
     - a typo
     - conversion damage
     - broken grammar
     - misleading due to obsolete formatting
   - do not smooth language if doing so risks changing technical
     meaning
   - Editorial improvements in the believed-correct existing
     `simh-docs` conversions should be performed on the uncorrected
     Pandoc conversions, including but not limited to:
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

5. For tables:
   - convert aligned text to real Markdown tables when that improves
     rendering
   - keep intentional blank separator rows inside tables when they
     preserve grouping from the original
   - remove stray leading or trailing blank rows that do not serve a
     purpose
   - do not over-normalize minimalist tables when the simpler form
     still reads well

6. Replace the Word documents only after the Markdown replacement is
   good enough to stand on its own.
   The practical sequence for each document should be:
   - finish the cleaned `simh-docs` version first
   - place it in its final location in this repository
   - update references and indexes as needed
   - update the tracker destination and notes
   - only then remove or retire the Word file from active use

   As a default placement policy:
   - simulator manuals should go under `docs/simulators/`
   - SIMH-wide manuals should go under `docs/project/`
   - standalone articles should go under `docs/articles/`
   - historical standalone pieces if any should go under `docs/history/`

   Adopted filenames should usually drop `_doc` unless that suffix is
   actually useful. Prefer short descriptive names and update the
   tracker when the final adopted name differs from the Word filename
   or the `simh-docs` draft name.

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

9. Before committing an adopted document:
   - reread the cleaned Markdown as Markdown, not just as a diff
   - check the front matter, TOC, tables, and example blocks
   - confirm the tracker entry accurately describes what changed
   - keep the working `simh-docs` copy in sync with the adopted version
     unless there is a specific reason not to


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
