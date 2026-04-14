# Document Conversion Tracker

This tracker records the status of each legacy Microsoft Word document
as it is reviewed, converted, proofread, and replaced.

Status values:

- `not started`: no adoption work has begun in this repository
- `imported`: Markdown draft copied or created, not yet proofread
- `proofread`: checked carefully against the Word original
- `replaced`: Markdown version is in place for active use
- `done`: follow-up cleanup is complete

Notes:

- Converted manuals should live under `docs/`, not under
  `simulators/`.
- The `Destination` column is still tentative in a few categorization
  cases, but not in the top-level policy sense.
- The `simh-docs` column reflects whether a counterpart exists there
  and whether its README describes it as proofread or raw Pandoc output.

| Source Word file | Destination | `simh-docs` | Status | Notes |
| --- | --- | --- | --- | --- |
| `Summary of IMP IO Device Codes.doc` | `docs/simulators/H316/h316_imp_IO_Device_Codes.md` | yes, cleaned and proofread | `replaced` | adopted from `simh-docs/docs/h316_imp_IO_Device_Codes.md`; compared carefully against the Word text export and the H316 IMP listing, corrected verified host-interface opcode and DMC pointer-name errors, converted malformed HTML/Markdown tables to normal Markdown tables, normalized heading structure, and converted extracted figures to working Markdown image references |
| `altairz80_doc.docx` | `docs/simulators/AltairZ80/altairz80.md` | yes, cleaned and proofread | `replaced` | adopted from `simh-docs/docs/altairz80_doc.md`; real linked TOC retained, simulator-files table cleaned, remaining section-by-section mechanical fixes applied, and revision-history ordering corrected before replacement |
| `b5500_doc.doc` | `docs/simulators/B5500/b5500_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `gri_doc.doc` | `docs/simulators/GRI/gri_doc.md` | yes, proofread | `replaced` | verified against Word; fixed raw TOC/page-number artifacts, repaired CPU option and HSP device-description errors from the draft, and cleaned prose and symbolic-syntax formatting before adoption |
| `h316_doc.doc` | `docs/simulators/H316/h316_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `h316_imp.doc` | `docs/simulators/H316/h316_imp.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `hp2100_doc.doc` | `docs/simulators/HP2100/hp2100_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `hp3000_doc.doc` | `docs/simulators/HP3000/hp3000_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `i1401_doc.doc` | `docs/simulators/I1401/i1401_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `i1620_doc.doc` | `docs/simulators/I1620/i1620_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `i650_doc.doc` | `docs/simulators/I650/i650_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `i7010_doc.doc` | `docs/simulators/I7000/i7010_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `i701_doc.doc` | `docs/simulators/I7000/i701_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `i7070_doc.doc` | `docs/simulators/I7000/i7070_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `i7080_doc.doc` | `docs/simulators/I7000/i7080_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `i7090_doc.doc` | `docs/simulators/I7094/i7090_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `i7094_doc.doc` | `docs/simulators/I7094/i7094_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `ibm1130.doc` | `docs/simulators/Ibm1130/ibm1130.md` | yes, raw pandoc | `not started` | basename differs from Word file |
| `id_doc.doc` | `docs/simulators/Interdata/id_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `ka10_doc.doc` | `docs/simulators/PDP10/ka10.md` | yes, proofread | `replaced` | verified against Word and cleaned before adoption; normalized front matter, repaired shared PDP-10-family prose and table formatting, corrected verified simulator-file and PDP-6 device references, and kept historically intentional wording where appropriate |
| `ki10_doc.doc` | `docs/simulators/PDP10/ki10.md` | yes, proofread | `replaced` | verified against Word and cleaned before adoption; corrected title/front matter, normalized command and feature tables, repaired formatting damage in device sections and TOC entries, and aligned the introduction wording with the cleaned PDP-10-family manuals |
| `kl10_doc.doc` | `docs/simulators/PDP10/kl10.md` | yes, raw pandoc | `replaced` | verified against Word; front matter, tables, symbolic syntax, and KL10-specific front-end/networking details cleaned for adoption |
| `ks10_doc.doc` | `docs/simulators/PDP10/ks10.md` | yes, proofread | `replaced` | verified against Word and cleaned before adoption; corrected front matter and TOC artifacts, fixed verified KS10 device naming (`DZ11`, `CH11`) and related prose, and normalized tables and shared introduction wording to match the cleaned PDP-10-family manuals |
| `lgp_doc.doc` | `docs/simulators/LGP/lgp_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `nova_doc.doc` | `docs/simulators/NOVA/nova_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `pdp10_doc.doc` | `docs/simulators/PDP10/pdp10.md` | yes, proofread | `replaced` | verified against Word and cleaned before adoption; split the malformed `IND_MAX`/`XCT_MAX` register rows, repaired broken `DZ`, `RP`, `TU`, `LP20`, `RY`, `CD20`, and symbolic-display formatting, corrected real prose and conversion defects, and preserved historical/shared PDP-10-family wording where it appeared intentional |
| `pdp11_doc.doc` | `docs/simulators/PDP11/pdp11.md` | yes, proofread | `replaced` | proofread against Word; TM11 table/prose bug fixed before adoption |
| `pdp18b_doc.doc` | `docs/simulators/PDP18B/pdp18b.md` | yes, cleaned and proofread | `replaced` | simulator manual under `docs/`; verified against Word and reworked substantially from raw Pandoc output |
| `pdp1_doc.doc` | `docs/simulators/PDP1/pdp1.md` | yes, raw pandoc | `replaced` | verified against Word and heavily cleaned before adoption; replaced fake page-number TOC entries, normalized front matter, converted command/register/mode listings into real Markdown tables, converted standalone syntax forms into fenced code blocks, aligned literal formatting with cleaned manuals, clarified PDP-1 glyph names with Unicode where unambiguous, and fixed real content defects including clock command names, a `WRE` register-name typo, dash corruption, and prose issues |
| `pdp6_doc.doc` | `docs/simulators/PDP10/pdp6.md` | yes, proofread | `replaced` | verified against Word and cleaned before adoption; normalized front matter and heading structure, converted raw command listings into Markdown tables, fixed verified technical mistakes including the `PDP7`/`PDP-6` terminal-multiplexer file-list typo and the symbolic-section `KA10`/`PDP-6` wording, and repaired numerous prose and formatting issues while preserving shared PDP-10-family wording where it appeared intentional |
| `pdp8_doc.doc` | `docs/simulators/PDP8/pdp8.md` | yes, proofread | `replaced` | verified against Word and cleaned before adoption; fixed the `7P` terminal-mode typo and the `LOAD` switch wording, normalized rough command/switch tables, removed blank spacer rows from error tables, cleaned symbolic display/input formatting, and repaired numerous prose and formatting issues while preserving the established content |
| `sds_doc.doc` | `docs/simulators/SDS/sds_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `sel32_doc.doc` | `docs/simulators/SEL32/sel32_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `sigma_doc.doc` | `docs/simulators/sigma/sigma_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `simh.doc` | `docs/developers/writing_a_simulator.md` | yes, proofread | `replaced` | verified against Word; fixed malformed VM hardware table, stray table rule, and obvious source typos before adoption |
| `simh_breakpoints.doc` | `docs/developers/breakpoints.md` | yes, proofread | `replaced` | verified against Word; fixed command dashes, code quote corruption, code formatting defects, and obvious source typos before adoption |
| `simh_doc.doc` | `docs/manual/users_guide.md` | yes, proofread | `replaced` | verified against Word as a cleaned historical document; fixed malformed command syntax, missing words, grammar/typo defects, and other conversion damage while preserving prior human editorial updates |
| `simh_faq.doc` | `docs/manual/faq.md` | yes, proofread | `replaced` | verified against Word as a cleaned historical document; fixed quote corruption, malformed examples, missing punctuation/spacing, and obvious grammar/typo defects while preserving prior human editorial updates |
| `simh_magtape.doc` | `docs/manual/magtape_format.md` | yes, proofread | `replaced` | verified against Word and adopted without substantive text changes |
| `simh_swre.doc` | `docs/manual/sample_software.md` | yes, proofread | `replaced` | verified against Word as a cleaned historical document; fixed command dash corruption, spelling/typo defects, and minor heading/text cleanup before adoption |
| `simh_vmio.doc` | `docs/developers/adding_io_devices.md` | yes, proofread | `replaced` | verified against Word as a cleaned historical document; fixed heading spacing, prose defects, I/O notation consistency, and a real `IVCL` code-example typo before adoption |
| `simulators_acm_queue_2004.doc` | `docs/articles/simulators_acm_queue_2004.md` | yes, proofread | `replaced` | verified against Word as a cleaned historical article; removed a stray heading artifact and fixed minor capitalization/unit/style defects while preserving the established hand-converted structure |
| `ssem_doc.doc` | `docs/simulators/SSEM/ssem.md` | yes, proofread | `replaced` | verified against Word; removed fake TOC/page-number artifacts, normalized front matter and small tables, and cleaned minor prose issues while preserving the document's minimalist style where it still read well |
| `swtp6800_doc.doc` | `docs/simulators/swtp6800/swtp6800_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `tx0_doc.doc` | `docs/simulators/TX-0/tx0_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `vax780_doc.doc` | `docs/simulators/VAX/vax780.md` | yes, proofread | `replaced` | verified against Word; corrected lingering XU/XQ and MCTL1 typos and removed stray placeholder comments before adoption |
| `vax_doc.doc` | `docs/simulators/VAX/vax.md` | yes, proofread | `replaced` | verified against Word; fixed stray `:smile:` conversion in XQ section before adoption |
