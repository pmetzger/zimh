# Documentation Style Guide

This file records the current house style for Markdown documentation
work in this repository, especially for Word-to-Markdown conversion
cleanup in `simh-docs/docs/`.

## Scope

- Preserve technical meaning and historically meaningful wording.
- Do structural cleanup and format repair
- Editorial and copy edit fixes are in scope, but anything beyond
  extremely obvious stuff ("it's" -> "its" or the like) needs to be
  cleared with human review.
- Treat non-mechanical wording or presentation changes as a human-review
  item unless the change is clearly required to fix conversion damage.

## Headings

- Use real Markdown headings for document structure.
- Use `#`, `##`, `###`, etc. for actual sections and subsections.
- The `Copyright Notice` line should use bold text.
- Do not promote labels to headings unless they are functioning as real
  document structure.

## Copyright And Front Matter

- Keep the title, revision line, and copyright section in the document's
  established house style.
- "Front matter" in this project means the human-readable leading
  material such as title, revision date, copyright notice, descriptive
  opening text, and TOC.
- Do not introduce YAML front matter.

## Table Of Contents

- Replace fake page-number TOCs with linked Markdown TOCs.
- Preserve a usable TOC if the file already has one.

## Commands And Code-Like Content

- A command or example that stands alone on its own line should normally
  remain a standalone code block, usually fenced with triple backticks.
- Do not turn a standalone command example or transcript into ordinary
  paragraph text; use three backquote fencing instead.
- Use inline code when a command or token is part of a sentence. Don't
  use ordinary quote marks for commands or tokens.
- Use inline code not only for commands, but also for literal device
  names, register names, flags, switches, file names, placeholder
  values, and other technical identifiers when they are being referred
  to literally.
- Treat grammar fragments and symbolic syntax forms as literal notation.
  Short forms such as `opcode ac, operand`, `ttss`, `-R`, `-w`, or
  `SET CPU NORMAL` should stay visibly code-like even when they are not
  full command examples.
- When several command lines are presented as a list of examples, prefer
  a fenced code block unless the source is clearly better represented as
  a command/action table.
- Use command/action tables only when the source material is genuinely a
  paired mapping between a command and its effect.
- Preserve literal placeholders such as `<file>`, `%xy`, `(idx)`, or
  similar syntax in code form.
- Use plain ASCII hyphen-minus in commands and especially flags unless
  the source genuinely requires another character.
- Real em-dashes should be used in English text when they are used as
  text punctuation.

## Tables

- Convert raw text tables or broken HTML tables into valid Markdown
  tables when practical.
- Table headers do not require backquotes just because they name a
  command or register category.
- Put literal punctuation or code-like symbols inside code spans when
  that avoids Markdown escaping noise.
- Remove conversion debris such as dummy first rows that really contain
  headers, malformed separator rows, or stray footnote fragments.
- Keep meaningful multi-row groupings if they help preserve the original
  structure.
- Prefer compact reference tables for stable mappings such as
  command/action, switch/effect, name/meaning, device/simulation, or
  character-code equivalences when the source material is reference
  oriented.
- Footnote like material can be moved to distinct tables or turned
  into GFM footnotes.

## Registers, Options, And Error Handling

- Register lists, command/action mappings, and error-handling matrices
  may be converted to Markdown tables when that improves readability and
  matches the source structure.
- Verify technical identifiers against source code or supporting source
  material when the converted draft appears wrong.

## Images

- Prefer Markdown image syntax over raw `<img>` tags.
- Use local relative asset paths that match the actual document asset
  directory.
- Add concise, useful alt text.
- Remove Word-derived inline `style=` sizing unless there is a specific
  reason to preserve HTML.
- If an image is embedded inside a larger structure such as a table or a
  numbered procedure, keep the surrounding structure readable in normal
  Markdown.

## Inline Symbols And Character Tables

- In character-set or code tables, prefer literal symbols in code spans
  over stray backslash escapes introduced by conversion.
- Keep symbol meaning explicit, especially for characters such as
  `` `#` ``, `` `|` ``, `` `[` ``, `` `]` ``, `` `<` ``, `` `>` ``, and
  backslash.

## Quote Marks

- Prefer curly quote marks in English text where it makes sense, e.g.,
  "A record can “grow” when..." is sensible. "don’t" is sensible.

## Miscellaneous

- “command line” is preferred to “command-line”.

## Human Review Boundary

- Mechanical fixes may be applied directly.
- Changes that alter presentation style beyond obvious cleanup should be
  cleared through a human-review tracker item first.
- In particular, if a standalone code example might be reformatted into
  another presentation style, treat that as a style-sensitive change and
  keep or restore the standalone block form unless a human approves a
  different representation.
