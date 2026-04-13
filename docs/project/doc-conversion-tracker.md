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
| `Summary of IMP IO Device Codes.doc` | `docs/simulators/H316/h316_imp_IO_Device_Codes.md` | yes, raw pandoc | `not started` | special filename; map carefully |
| `altairz80_doc.docx` | `docs/simulators/AltairZ80/altairz80_doc.md` | yes, raw pandoc | `not started` | newly generated Pandoc draft in `simh-docs` |
| `b5500_doc.doc` | `docs/simulators/B5500/b5500_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `gri_doc.doc` | `docs/simulators/GRI/gri_doc.md` | yes, proofread | `not started` | verify against Word before adopting |
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
| `ka10_doc.doc` | `docs/simulators/PDP10/ka10_doc.md` | yes, proofread | `not started` | verify against Word before adopting |
| `ki10_doc.doc` | `docs/simulators/PDP10/ki10_doc.md` | yes, proofread | `not started` | verify against Word before adopting |
| `kl10_doc.doc` | `docs/simulators/PDP10/kl10_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `ks10_doc.doc` | `docs/simulators/PDP10/ks10_doc.md` | yes, proofread | `not started` | verify against Word before adopting |
| `lgp_doc.doc` | `docs/simulators/LGP/lgp_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `nova_doc.doc` | `docs/simulators/NOVA/nova_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `pdp10_doc.doc` | `docs/simulators/PDP10/pdp10_doc.md` | yes, proofread | `not started` | verify relationship to KA10/KI10/KS10 docs |
| `pdp11_doc.doc` | `docs/simulators/PDP11/pdp11.md` | yes, proofread | `replaced` | proofread against Word; TM11 table/prose bug fixed before adoption |
| `pdp18b_doc.doc` | `docs/simulators/PDP18B/pdp18b_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `pdp1_doc.doc` | `docs/simulators/PDP1/pdp1_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `pdp6_doc.doc` | `docs/simulators/PDP10/pdp6_doc.md` | yes, proofread | `not started` | tentative placement under PDP10 family |
| `pdp8_doc.doc` | `docs/simulators/PDP8/pdp8_doc.md` | yes, proofread | `not started` | verify against Word before adopting |
| `sds_doc.doc` | `docs/simulators/SDS/sds_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `sel32_doc.doc` | `docs/simulators/SEL32/sel32_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `sigma_doc.doc` | `docs/simulators/sigma/sigma_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `simh.doc` | `docs/project/simh.md` | yes, proofread | `not started` | writing a SIMH simulator |
| `simh_breakpoints.doc` | `docs/project/simh_breakpoints.md` | yes, proofread | `not started` | project-level documentation |
| `simh_doc.doc` | `docs/project/simh_doc.md` | yes, proofread | `not started` | SIMH user's guide |
| `simh_faq.doc` | `docs/project/simh_faq.md` | yes, proofread | `not started` | project-level documentation |
| `simh_magtape.doc` | `docs/project/simh_magtape.md` | yes, proofread | `not started` | project-level documentation |
| `simh_swre.doc` | `docs/project/simh_swre.md` | yes, proofread | `not started` | sample software documentation |
| `simh_vmio.doc` | `docs/project/simh_vmio.md` | yes, proofread | `not started` | project-level documentation |
| `simulators_acm_queue_2004.doc` | `docs/articles/simulators_acm_queue_2004.md` | yes, proofread | `not started` | article rather than manual |
| `ssem_doc.doc` | `docs/simulators/SSEM/ssem_doc.md` | yes, proofread | `not started` | verify against Word before adopting |
| `swtp6800_doc.doc` | `docs/simulators/swtp6800/swtp6800_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `tx0_doc.doc` | `docs/simulators/TX-0/tx0_doc.md` | yes, raw pandoc | `not started` | simulator manual under `docs/` |
| `vax780_doc.doc` | `docs/simulators/VAX/vax780.md` | yes, proofread | `replaced` | verified against Word; corrected lingering XU/XQ and MCTL1 typos and removed stray placeholder comments before adoption |
| `vax_doc.doc` | `docs/simulators/VAX/vax.md` | yes, proofread | `replaced` | verified against Word; fixed stray `:smile:` conversion in XQ section before adoption |
