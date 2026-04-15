# Build Warning Inventory

This file captures the warning inventory from a fresh GCC/Clang-style warning-enabled build of the repository.

Build used:

```sh
cmake -G Ninja -DWITH_VIDEO=Off -DCMAKE_BUILD_TYPE=Release -S . -B cmake/build-warnings-default
ninja -C cmake/build-warnings-default -j 8
```

Notes:

- `WITH_VIDEO=Off` was used because this environment is missing the SDL_ttf development headers required for a video-enabled build.
- Raw configure log: `tmp/warnings-configure.log`
- Raw build log: `tmp/warnings-build.log`
- Counts below collapse repeated warnings emitted from multiple build variants of the same source location.

## Summary

- Raw warning diagnostics: `61621`
- Unique warning sites: `27217`
- Files with warnings: `711`
- Warning classes: `19`

## Warning Classes

| Warning flag | Unique sites |
|---|---:|
| `-Wmissing-field-initializers` | 15760 |
| `-Wunused-parameter` | 6454 |
| `-Wmissing-prototypes` | 3539 |
| `-Wimplicit-fallthrough` | 546 |
| `-Wstrict-prototypes` | 347 |
| `-Wundef` | 128 |
| `-Wsign-compare` | 120 |
| `-Wmissing-braces` | 109 |
| `-Wunused-but-set-variable` | 75 |
| `-Wformat-nonliteral` | 55 |
| `-Wunused-function` | 35 |
| `-Wunused-variable` | 31 |
| `-Wbitwise-instead-of-logical` | 5 |
| `-Wmisleading-indentation` | 5 |
| `-Wsometimes-uninitialized` | 2 |
| `-Wstring-concatenation` | 2 |
| `-Wchar-subscripts` | 2 |
| `-Wcast-function-type-mismatch` | 1 |
| `-Wuninitialized` | 1 |

## Top Files

| File | Unique sites |
|---|---:|
| `simulators/AltairZ80/m68k/m68kopnz.c` | 663 |
| `simulators/AltairZ80/m68k/m68kopac.c` | 661 |
| `simulators/AltairZ80/m68k/m68kopdm.c` | 638 |
| `simulators/HP2100/hp2100_sys.c` | 577 |
| `simulators/PDP10/kx10_cpu.c` | 294 |
| `src/runtime/sim_console.c` | 233 |
| `src/core/scp.c` | 233 |
| `simulators/I7094/i7094_mt.c` | 211 |
| `simulators/PDP11/pdp11_cpu.c` | 202 |
| `simulators/HP3000/hp3000_sys.c` | 201 |
| `simulators/PDP11/pdp11_rq.c` | 197 |
| `simulators/PDP11/pdp11_xq.c` | 193 |
| `simulators/AltairZ80/altairz80_cpu.c` | 179 |
| `simulators/I7094/i7094_io.c` | 160 |
| `simulators/HP2100/hp2100_cpu.c` | 148 |
| `simulators/PDP11/pdp11_xu.c` | 143 |
| `simulators/sigma/sigma_io.c` | 141 |
| `simulators/BESM6/besm6_cpu.c` | 136 |
| `simulators/sigma/sigma_dp.c` | 133 |
| `simulators/PDP11/pdp11_dmc.c` | 131 |
| `simulators/B5500/b5500_cpu.c` | 129 |
| `simulators/Intel-Systems/common/sys.c` | 126 |
| `simulators/PDP18B/pdp18b_cpu.c` | 123 |
| `simulators/VAX/vax_sysdev.c` | 119 |
| `simulators/BESM6/besm6_disk.c` | 113 |
| `simulators/PDP10/kx10_tym.c` | 113 |
| `simulators/PDP10/kx10_rp.c` | 112 |
| `simulators/HP3000/hp3000_cpu.c` | 111 |
| `simulators/Interdata/id32_cpu.c` | 111 |
| `simulators/NOVA/eclipse_cpu.c` | 110 |
| `simulators/alpha/alpha_cpu.c` | 110 |
| `simulators/PDP11/pdp11_io_lib.c` | 108 |
| `simulators/sigma/sigma_cpu.c` | 106 |
| `simulators/HP3000/hp3000_atc.c` | 105 |
| `simulators/I7000/i7000_mt.c` | 105 |
| `simulators/VAX/vax750_stddev.c` | 105 |
| `simulators/I650/i650_sys.c` | 104 |
| `simulators/Interdata/id16_cpu.c` | 103 |
| `simulators/VAX/vax_cpu.c` | 103 |
| `simulators/sigma/sigma_coc.c` | 103 |
| `simulators/H316/h316_cpu.c` | 102 |
| `simulators/PDP18B/pdp18b_stddev.c` | 102 |
| `simulators/VAX/vax730_stddev.c` | 101 |
| `simulators/AltairZ80/altairz80_sio.c` | 98 |
| `simulators/GRI/gri_cpu.c` | 97 |
| `simulators/PDP10/kl10_fe.c` | 96 |
| `simulators/PDP11/pdp11_cpumod.c` | 95 |
| `simulators/HP3000/hp3000_lp.c` | 94 |
| `simulators/SAGE/sage_stddev.c` | 94 |
| `simulators/VAX/vax820_stddev.c` | 94 |

## Suggested Work Queue

- [ ] `-Wmissing-field-initializers`: 15760 unique sites. Biggest concentrations: `simulators/HP2100/hp2100_sys.c` (569), `simulators/I7094/i7094_mt.c` (211), `simulators/HP3000/hp3000_sys.c` (181), `simulators/PDP11/pdp11_cpu.c` (169), `simulators/PDP11/pdp11_rq.c` (168).
- [ ] `-Wunused-parameter`: 6454 unique sites. Biggest concentrations: `src/core/scp.c` (115), `simulators/PDP10/kx10_tym.c` (100), `src/runtime/sim_console.c` (85), `simulators/AltairZ80/altairz80_cpu.c` (75), `src/runtime/sim_video.c` (75).
- [ ] `-Wmissing-prototypes`: 3539 unique sites. Biggest concentrations: `simulators/AltairZ80/m68k/m68kopnz.c` (663), `simulators/AltairZ80/m68k/m68kopac.c` (661), `simulators/AltairZ80/m68k/m68kopdm.c` (638), `simulators/PDP10/kx10_cpu.c` (60), `simulators/PDP11/pdp11_dmc.c` (43).
- [ ] `-Wimplicit-fallthrough`: 546 unique sites. Biggest concentrations: `simulators/VAX/vax_cpu.c` (25), `simulators/PDP10/kx10_cpu.c` (22), `simulators/I7000/i7010_cpu.c` (12), `simulators/SEL32/sel32_cpu.c` (12), `simulators/SEL32/sel32_ipu.c` (12).
- [ ] `-Wstrict-prototypes`: 347 unique sites. Biggest concentrations: `simulators/3B2/3b2_mau.c` (51), `simulators/PDQ-3/pdq3_cpu.c` (22), `simulators/B5500/b5500_cpu.c` (21), `simulators/PDP18B/pdp18b_g2tty.c` (18), `simulators/3B2/3b2_cpu.c` (16).
- [ ] `-Wundef`: 128 unique sites. Biggest concentrations: `simulators/PDP10/kx10_sys.c` (52), `simulators/I7000/i7000_mt.c` (36), `simulators/PDP10/kx10_cpu.c` (17), `simulators/PDP10/kx10_defs.h` (7), `simulators/PDP10/ks10_kmc.c` (2).
- [ ] `-Wsign-compare`: 120 unique sites. Biggest concentrations: `third_party/slirp/socket.c` (13), `simulators/SEL32/sel32_ec.c` (12), `simulators/VAX/vax_cpu1.c` (10), `simulators/BESM6/besm6_mmu.c` (7), `simulators/I650/i650_mt.c` (6).
- [ ] `-Wmissing-braces`: 109 unique sites. Biggest concentrations: `simulators/Intel-Systems/common/sys.c` (108), `simulators/VAX/vax_uw.c` (1).
- [ ] `-Wunused-but-set-variable`: 75 unique sites. Biggest concentrations: `src/runtime/sim_scsi.c` (8), `src/runtime/sim_tmxr.c` (6), `simulators/I650/i650_cdp.c` (5), `simulators/Intel-Systems/common/isbc201.c` (5), `simulators/I650/i650_cdr.c` (3).
- [ ] `-Wformat-nonliteral`: 55 unique sites. Biggest concentrations: `simulators/PDP11/pdp11_xq.c` (12), `simulators/PDP11/pdp11_xu.c` (10), `simulators/BESM6/besm6_sys.c` (6), `simulators/3B2/3b2_ni.c` (5), `simulators/Ibm1130/ibm1130_cpu.c` (5).
- [ ] `-Wunused-function`: 35 unique sites. Biggest concentrations: `simulators/PDP11/pdp11_ddcmp.h` (10), `simulators/PDP10/ks10_dup.c` (2), `simulators/PDP10/ks10_kmc.c` (2), `simulators/PDP11/pdp11_dup.c` (2), `src/runtime/sim_sock.c` (2).
- [ ] `-Wunused-variable`: 31 unique sites. Biggest concentrations: `simulators/BESM6/besm6_disk.c` (6), `simulators/PDP10/ks10_dup.c` (3), `simulators/BESM6/besm6_mg.c` (2), `simulators/Intel-Systems/common/i8080.c` (2), `simulators/swtp6800/common/dc-4.c` (2).
- [ ] `-Wbitwise-instead-of-logical`: 5 unique sites. Biggest concentrations: `simulators/3B2/3b2_cpu.c` (5).
- [ ] `-Wmisleading-indentation`: 5 unique sites. Biggest concentrations: `simulators/Ibm1130/ibm1130_cpu.c` (1), `simulators/NOVA/nova_cpu.c` (1), `simulators/PDP18B/pdp18b_cpu.c` (1), `simulators/VAX/vax4xx_rz94.c` (1), `simulators/sigma/sigma_dk.c` (1).
- [ ] `-Wsometimes-uninitialized`: 2 unique sites. Biggest concentrations: `simulators/I650/i650_cpu.c` (2).
- [ ] `-Wstring-concatenation`: 2 unique sites. Biggest concentrations: `simulators/PDP10/pdp10_tu.c` (1), `simulators/PDP11/pdp11_tu.c` (1).
- [ ] `-Wchar-subscripts`: 2 unique sites. Biggest concentrations: `simulators/S3/s3_cd.c` (2).
- [ ] `-Wcast-function-type-mismatch`: 1 unique sites. Biggest concentrations: `src/runtime/sim_sock.c` (1).
- [ ] `-Wuninitialized`: 1 unique sites. Biggest concentrations: `simulators/SEL32/sel32_ipu.c` (1).

## Inventory By Warning Class

### `-Wmissing-field-initializers` (15760 unique sites)

Example sites:

- `simulators/3B2/3b2_cpu.c:173`: missing field 'action' initializer
- `simulators/3B2/3b2_cpu.c:203`: missing field 'offset' initializer
- `simulators/3B2/3b2_cpu.c:222`: missing field 'offset' initializer
- `simulators/3B2/3b2_cpu.c:233`: missing field 'offset' initializer
- `simulators/3B2/3b2_cpu.c:239`: missing field 'flags' initializer

| File | Unique sites |
|---|---:|
| `simulators/HP2100/hp2100_sys.c` | 569 |
| `simulators/I7094/i7094_mt.c` | 211 |
| `simulators/HP3000/hp3000_sys.c` | 181 |
| `simulators/PDP11/pdp11_cpu.c` | 169 |
| `simulators/PDP11/pdp11_rq.c` | 168 |
| `simulators/PDP10/kx10_cpu.c` | 147 |
| `src/runtime/sim_console.c` | 144 |
| `simulators/I7094/i7094_io.c` | 140 |
| `simulators/PDP11/pdp11_xq.c` | 119 |
| `simulators/sigma/sigma_dp.c` | 119 |
| `simulators/BESM6/besm6_cpu.c` | 105 |
| `simulators/PDP10/kx10_rp.c` | 105 |
| `simulators/sigma/sigma_io.c` | 103 |
| `simulators/HP2100/hp2100_cpu.c` | 102 |
| `simulators/AltairZ80/altairz80_cpu.c` | 97 |
| `simulators/HP3000/hp3000_atc.c` | 97 |
| `simulators/alpha/alpha_cpu.c` | 96 |
| `simulators/sigma/sigma_coc.c` | 96 |
| `simulators/BESM6/besm6_disk.c` | 88 |
| `simulators/Interdata/id32_cpu.c` | 88 |
| `simulators/PDP11/pdp11_io_lib.c` | 88 |
| `src/core/scp.c` | 88 |
| `simulators/PDP11/pdp11_xu.c` | 87 |
| `simulators/I650/i650_sys.c` | 85 |
| `simulators/PDP18B/pdp18b_cpu.c` | 85 |
| `simulators/HP2100/hp2100_mux.c` | 84 |
| `simulators/Interdata/id16_cpu.c` | 80 |
| `simulators/HP2100/hp2100_ds.c` | 79 |
| `simulators/I7094/i7094_com.c` | 79 |
| `simulators/HP3000/hp3000_cpu.c` | 78 |
| `simulators/HP3000/hp3000_lp.c` | 76 |
| `simulators/NOVA/eclipse_cpu.c` | 76 |
| `simulators/PDP18B/pdp18b_stddev.c` | 75 |
| `simulators/sigma/sigma_cpu.c` | 75 |
| `simulators/Intel-Systems/common/port.c` | 73 |
| `simulators/SAGE/sage_stddev.c` | 73 |
| `simulators/H316/h316_stddev.c` | 71 |
| `simulators/NOVA/nova_dkp.c` | 67 |
| `simulators/PDP10/pdp10_cpu.c` | 67 |
| `simulators/VAX/vax_sysdev.c` | 65 |
| `simulators/Interdata/id_pas.c` | 64 |
| `simulators/LGP/lgp_stddev.c` | 64 |
| `simulators/VAX/vax750_stddev.c` | 64 |
| `simulators/HP2100/hp2100_mpx.c` | 63 |
| `simulators/PDP18B/pdp18b_lp.c` | 63 |
| `simulators/HP2100/hp2100_ms.c` | 62 |
| `simulators/I7094/i7094_cpu.c` | 62 |
| `simulators/PDP11/pdp11_dup.c` | 61 |
| `simulators/PDP11/pdp11_rh.c` | 60 |
| `simulators/VAX/vax730_stddev.c` | 60 |
| `simulators/HP2100/hp2100_dp.c` | 59 |
| `simulators/PDP11/pdp11_hk.c` | 59 |
| `simulators/I7094/i7094_dsk.c` | 58 |
| `simulators/PDP1/pdp1_dcs.c` | 58 |
| `simulators/SDS/sds_mux.c` | 58 |
| `simulators/AltairZ80/altairz80_sio.c` | 57 |
| `simulators/HP3000/hp3000_scmb.c` | 57 |
| `simulators/I7000/i7080_cpu.c` | 57 |
| `simulators/PDP1/pdp1_stddev.c` | 57 |
| `simulators/Interdata/id_idc.c` | 56 |
| `simulators/PDP1/pdp1_cpu.c` | 56 |
| `simulators/VAX/vax780_stddev.c` | 56 |
| `simulators/VAX/vax820_stddev.c` | 56 |
| `simulators/VAX/vax_cpu.c` | 56 |
| `simulators/GRI/gri_cpu.c` | 55 |
| `simulators/I7000/i7000_mt.c` | 55 |
| `simulators/BESM6/besm6_mg.c` | 53 |
| `simulators/H316/h316_cpu.c` | 53 |
| `simulators/HP2100/hp2100_lpt.c` | 53 |
| `simulators/VAX/vax860_stddev.c` | 53 |
| `simulators/HP2100/hp2100_baci.c` | 52 |
| `simulators/PDP11/pdp11_cpumod.c` | 52 |
| `simulators/S3/s3_disk.c` | 52 |
| `simulators/CDC1700/cdc1700_dc.c` | 51 |
| `simulators/HP2100/hp2100_dq.c` | 51 |
| `simulators/NOVA/nova_cpu.c` | 51 |
| `simulators/PDP10/kl10_fe.c` | 51 |
| `simulators/SDS/sds_cpu.c` | 51 |
| `simulators/BESM6/besm6_tty.c` | 50 |
| `simulators/HP3000/hp3000_ds.c` | 50 |
| `simulators/I7000/i7090_cpu.c` | 50 |
| `simulators/AltairZ80/s100_2sio.c` | 49 |
| `simulators/AltairZ80/s100_jair.c` | 49 |
| `simulators/GRI/gri_stddev.c` | 49 |
| `simulators/I7000/i7000_com.c` | 49 |
| `simulators/Intel-Systems/common/isbc208.c` | 49 |
| `simulators/PDP8/pdp8_ttx.c` | 49 |
| `simulators/BESM6/besm6_mmu.c` | 48 |
| `simulators/PDP11/pdp11_tq.c` | 48 |
| `simulators/B5500/b5500_cpu.c` | 47 |
| `simulators/I1401/i1401_cpu.c` | 47 |
| `simulators/PDP10/kx10_dp.c` | 47 |
| `simulators/PDP11/pdp11_dmc.c` | 47 |
| `simulators/PDP11/pdp11_stddev.c` | 47 |
| `simulators/PDP18B/pdp18b_tt1.c` | 47 |
| `simulators/H316/h316_dp.c` | 46 |
| `simulators/SEL32/sel32_cpu.c` | 46 |
| `simulators/SEL32/sel32_ipu.c` | 46 |
| `simulators/HP2100/hp2100_pt.c` | 45 |
| `simulators/PDP11/pdp11_rp.c` | 45 |
| `simulators/S3/s3_cpu.c` | 45 |
| `src/runtime/sim_timer.c` | 45 |
| `simulators/CDC1700/cdc1700_dev1.c` | 44 |
| `simulators/I7000/i7090_chan.c` | 44 |
| `simulators/TX-0/tx0_stddev.c` | 44 |
| `simulators/AltairZ80/s100_dj2d.c` | 43 |
| `simulators/HP2100/hp2100_dr.c` | 43 |
| `simulators/3B2/3b2_cpu.c` | 42 |
| `simulators/I7000/i7000_dsk.c` | 42 |
| `simulators/PDP11/pdp11_dl.c` | 42 |
| `simulators/PDP8/pdp8_cpu.c` | 42 |
| `simulators/VAX/vax7x0_mba.c` | 42 |
| `simulators/AltairZ80/s100_adcs6.c` | 41 |
| `simulators/I7000/i7080_chan.c` | 41 |
| `simulators/PDQ-3/pdq3_cpu.c` | 41 |
| `simulators/H316/h316_fhd.c` | 40 |
| `simulators/PDP10/pdp10_rp.c` | 40 |
| `simulators/PDP11/pdp11_dc.c` | 40 |
| `simulators/SAGE/m68k_cpu.c` | 40 |
| `simulators/TX-0/tx0_cpu.c` | 40 |
| `simulators/AltairZ80/s100_ss1.c` | 39 |
| `simulators/I7000/i7010_cpu.c` | 39 |
| `simulators/PDP11/pdp11_tc.c` | 39 |
| `simulators/PDP18B/pdp18b_dt.c` | 39 |
| `simulators/SEL32/sel32_com.c` | 39 |
| `simulators/VAX/vax_stddev.c` | 39 |
| `simulators/alpha/alpha_ev5_pal.c` | 39 |
| `simulators/Ibm1130/ibm1130_cpu.c` | 38 |
| `simulators/PDP8/pdp8_rx.c` | 38 |
| `simulators/PDQ-3/pdq3_fdc.c` | 38 |
| `simulators/SDS/sds_stddev.c` | 38 |
| `simulators/VAX/vax780_uba.c` | 38 |
| `simulators/sigma/sigma_mt.c` | 38 |
| `simulators/AltairZ80/sol20.c` | 37 |
| `simulators/AltairZ80/wd179x.c` | 37 |
| `simulators/I7000/i7070_chan.c` | 37 |
| `simulators/Interdata/id_dp.c` | 37 |
| `simulators/PDP10/ks10_dup.c` | 37 |
| `simulators/PDP11/pdp11_rr.c` | 37 |
| `simulators/VAX/vax610_stddev.c` | 37 |
| `simulators/VAX/vax630_stddev.c` | 37 |
| `simulators/AltairZ80/altairz80_dsk.c` | 36 |
| `simulators/HP2100/hp2100_tty.c` | 36 |
| `simulators/HP3000/hp3000_ms.c` | 36 |
| `simulators/PDP10/kx10_mt.c` | 36 |
| `simulators/PDP11/pdp11_pt.c` | 36 |
| `simulators/PDP8/pdp8_dt.c` | 36 |
| `simulators/PDP8/pdp8_rl.c` | 36 |
| `simulators/3B2/3b2_iu.c` | 35 |
| `simulators/AltairZ80/mmd.c` | 35 |
| `simulators/HP2100/hp2100_lps.c` | 35 |
| `simulators/HP2100/hp2100_mt.c` | 35 |
| `simulators/I1401/i1401_cd.c` | 35 |
| `simulators/PDP10/pdp10_tu.c` | 35 |
| `simulators/PDP11/pdp11_rk.c` | 35 |
| `simulators/HP2100/hp2100_ipl.c` | 34 |
| `simulators/LGP/lgp_cpu.c` | 34 |
| `simulators/PDP10/pdp10_lp20.c` | 34 |
| `simulators/PDQ-3/pdq3_stddev.c` | 34 |
| `simulators/H316/h316_mt.c` | 33 |
| `simulators/HP2100/hp2100_mem.c` | 33 |
| `simulators/Interdata/id_uvc.c` | 33 |
| `simulators/PDP1/pdp1_dt.c` | 33 |
| `simulators/PDP10/kx10_tu.c` | 33 |
| `simulators/PDP11/pdp11_ry.c` | 33 |
| `simulators/HP2100/hp2100_dma.c` | 32 |
| `simulators/PDP11/pdp11_ts.c` | 32 |
| `simulators/ALTAIR/altair_cpu.c` | 31 |
| `simulators/AltairZ80/s100_tuart.c` | 31 |
| `simulators/I1620/i1620_cpu.c` | 31 |
| `simulators/NOVA/nova_tt1.c` | 31 |
| `simulators/VAX/vax820_uba.c` | 31 |
| `simulators/I650/i650_cpu.c` | 30 |
| `simulators/I7094/i7094_cd.c` | 30 |
| `simulators/Ibm1130/ibm1130_cr.c` | 30 |
| `simulators/Intel-Systems/common/i8251.c` | 30 |
| `simulators/NOVA/nova_qty.c` | 30 |
| `simulators/PDP11/pdp11_vh.c` | 30 |
| `simulators/SAGE/sage_cons.c` | 30 |
| `simulators/VAX/vax_xs.c` | 30 |
| `simulators/AltairZ80/s100_hayes.c` | 29 |
| `simulators/HP2100/hp2100_mc.c` | 29 |
| `simulators/HP3000/hp3000_mpx.c` | 29 |
| `simulators/PDP11/pdp11_td.c` | 29 |
| `simulators/imlac/imlac_cpu.c` | 29 |
| `simulators/AltairZ80/s100_pmmi.c` | 28 |
| `simulators/HP3000/hp3000_sel.c` | 28 |
| `simulators/I7000/i7070_cpu.c` | 28 |
| `simulators/Intel-Systems/common/i8080.c` | 28 |
| `simulators/Intel-Systems/common/i8255.c` | 28 |
| `simulators/NOVA/nova_pt.c` | 28 |
| `simulators/PDP11/pdp11_pclk.c` | 28 |
| `simulators/PDP11/pdp11_rx.c` | 28 |
| `simulators/PDP8/pdp8_mt.c` | 28 |
| `simulators/AltairZ80/s100_tarbell.c` | 27 |
| `simulators/I7000/i7000_cdp.c` | 27 |
| `simulators/Interdata/id_mt.c` | 27 |
| `simulators/Interdata/id_ttp.c` | 27 |
| `simulators/PDP1/pdp1_drm.c` | 27 |
| `simulators/PDP10/ka10_ai.c` | 27 |
| `simulators/PDP10/kx10_rs.c` | 27 |
| `simulators/PDP11/pdp11_dz.c` | 27 |
| `simulators/PDP11/pdp11_tm.c` | 27 |
| `simulators/S3/s3_cd.c` | 27 |
| `simulators/SDS/sds_mt.c` | 27 |
| `simulators/linc/linc_cpu.c` | 27 |
| `simulators/AltairZ80/altairz80_hdsk.c` | 26 |
| `simulators/HP3000/hp3000_clk.c` | 26 |
| `simulators/Interdata/id_tt.c` | 26 |
| `simulators/NOVA/nova_tt.c` | 26 |
| `simulators/PDP11/pdp11_cr.c` | 26 |
| `simulators/PDP18B/pdp18b_rf.c` | 26 |
| `simulators/imlac/imlac_dp.c` | 26 |
| `simulators/tt2500/tt2500_cpu.c` | 26 |
| `simulators/B5500/b5500_dk.c` | 25 |
| `simulators/H316/h316_rtc.c` | 25 |
| `simulators/HP2100/hp2100_di_da.c` | 25 |
| `simulators/I7000/i7010_chan.c` | 25 |
| `simulators/Interdata/id_fd.c` | 25 |
| `simulators/NOVA/nova_mta.c` | 25 |
| `simulators/PDP10/kx10_dt.c` | 25 |
| `simulators/PDP11/pdp11_rl.c` | 25 |
| `simulators/PDP11/pdp11_rs.c` | 25 |
| `simulators/PDP18B/pdp18b_rp.c` | 25 |
| `simulators/PDP8/pdp8_td.c` | 25 |
| `simulators/VAX/vax4xx_dz.c` | 25 |
| `simulators/VAX/vax4xx_rz80.c` | 25 |
| `simulators/Intel-Systems/common/i8008.c` | 24 |
| `simulators/PDP18B/pdp18b_g2tty.c` | 24 |
| `simulators/PDP18B/pdp18b_mt.c` | 24 |
| `simulators/PDP8/pdp8_ct.c` | 24 |
| `simulators/PDP8/pdp8_pt.c` | 24 |
| `simulators/VAX/is1000_sysdev.c` | 24 |
| `simulators/VAX/vax4xx_rd.c` | 24 |
| `simulators/VAX/vax4xx_stddev.c` | 24 |
| `simulators/sigma/sigma_lp.c` | 24 |
| `simulators/AltairZ80/ibc.c` | 23 |
| `simulators/Intel-Systems/common/zx200a.c` | 23 |
| `simulators/PDP11/pdp11_ta.c` | 23 |
| `simulators/PDP8/pdp8_rf.c` | 23 |
| `simulators/VAX/vax_vc.c` | 23 |
| `simulators/AltairZ80/s100_icom.c` | 22 |
| `simulators/AltairZ80/s100_scp300f.c` | 22 |
| `simulators/CDC1700/cdc1700_mt.c` | 22 |
| `simulators/H316/h316_mi.c` | 22 |
| `simulators/HP2100/hp2100_tbg.c` | 22 |
| `simulators/Interdata/id_pt.c` | 22 |
| `simulators/ND100/nd100_stddev.c` | 22 |
| `simulators/NOVA/eclipse_tt.c` | 22 |
| `simulators/NOVA/nova_dsk.c` | 22 |
| `simulators/PDP10/pdp6_dtc.c` | 22 |
| `simulators/PDP11/pdp11_kmc.c` | 22 |
| `simulators/PDP11/pdp11_tu.c` | 22 |
| `simulators/PDP8/pdp8_fpp.c` | 22 |
| `simulators/PDP8/pdp8_tt.c` | 22 |
| `simulators/VAX/vax750_uba.c` | 22 |
| `simulators/sigma/sigma_rad.c` | 22 |
| `simulators/I7094/i7094_lp.c` | 21 |
| `simulators/Ibm1130/ibm1130_sca.c` | 21 |
| `simulators/Ibm1130/ibm1130_stddev.c` | 21 |
| `simulators/Interdata/id_lp.c` | 21 |
| `simulators/ND100/nd100_cpu.c` | 21 |
| `simulators/PDP10/ks10_kmc.c` | 21 |
| `simulators/PDP11/pdp11_rc.c` | 21 |
| `simulators/PDP11/pdp11_rf.c` | 21 |
| `simulators/PDP8/pdp8_df.c` | 21 |
| `simulators/SEL32/sel32_clk.c` | 21 |
| `simulators/sigma/sigma_dk.c` | 21 |
| `simulators/H316/h316_lp.c` | 20 |
| `simulators/I7094/i7094_drm.c` | 20 |
| `simulators/Intel-Systems/common/isbc202.c` | 20 |
| `simulators/PDP11/pdp11_uc15.c` | 20 |
| `simulators/SDS/sds_io.c` | 20 |
| `simulators/VAX/vax630_sysdev.c` | 20 |
| `simulators/swtp6800/common/mp-a2.c` | 20 |
| `simulators/ALTAIR/altair_sio.c` | 19 |
| `simulators/AltairZ80/ibc_mcc_hdc.c` | 19 |
| `simulators/AltairZ80/s100_disk3.c` | 19 |
| `simulators/B5500/b5500_io.c` | 19 |
| `simulators/B5500/b5500_mt.c` | 19 |
| `simulators/I1401/i1401_mt.c` | 19 |
| `simulators/Interdata/id_io.c` | 19 |
| `simulators/PDP10/kx10_rc.c` | 19 |
| `simulators/SAGE/sage_lp.c` | 19 |
| `simulators/VAX/vax780_sbi.c` | 19 |
| `simulators/AltairZ80/s100_djhdc.c` | 18 |
| `simulators/AltairZ80/s100_hdc1001.c` | 18 |
| `simulators/I1401/i1401_dp.c` | 18 |
| `simulators/I1401/i1401_lp.c` | 18 |
| `simulators/I7000/i701_cpu.c` | 18 |
| `simulators/Intel-Systems/common/isbc206.c` | 18 |
| `simulators/PDP10/ks10_lp.c` | 18 |
| `simulators/PDP10/pdp10_ksio.c` | 18 |
| `simulators/PDP18B/pdp18b_dr15.c` | 18 |
| `simulators/PDP8/pdp8_rk.c` | 18 |
| `simulators/SDS/sds_lp.c` | 18 |
| `simulators/SEL32/sel32_ec.c` | 18 |
| `simulators/VAX/vax4nn_stddev.c` | 18 |
| `simulators/VAX/vax730_uba.c` | 18 |
| `simulators/VAX/vax780_mem.c` | 18 |
| `simulators/sigma/sigma_tt.c` | 18 |
| `simulators/CDC1700/cdc1700_cd.c` | 17 |
| `simulators/CDC1700/cdc1700_cpu.c` | 17 |
| `simulators/I7000/i7090_sys.c` | 17 |
| `simulators/Intel-Systems/common/isbc201.c` | 17 |
| `simulators/PDP18B/pdp18b_fpp.c` | 17 |
| `simulators/S3/s3_lp.c` | 17 |
| `simulators/VAX/vax4xx_vc.c` | 17 |
| `simulators/VAX/vax_rzdev.h` | 17 |
| `simulators/sigma/sigma_pt.c` | 17 |
| `simulators/3B2/3b2_rev2_mmu.c` | 16 |
| `simulators/AltairZ80/ibc_smd_hdc.c` | 16 |
| `simulators/AltairZ80/s100_disk2.c` | 16 |
| `simulators/H316/h316_hi.c` | 16 |
| `simulators/I1620/i1620_lp.c` | 16 |
| `simulators/Ibm1130/ibm1130_disk.c` | 16 |
| `simulators/Intel-Systems/common/i8253.c` | 16 |
| `simulators/NOVA/nova_clk.c` | 16 |
| `simulators/PDP1/pdp1_lp.c` | 16 |
| `simulators/PDP10/pdp10_tim.c` | 16 |
| `simulators/PDP10/pdp6_mtc.c` | 16 |
| `simulators/PDP11/pdp11_dh.c` | 16 |
| `simulators/VAX/vax_watch.c` | 16 |
| `simulators/sigma/sigma_rtc.c` | 16 |
| `simulators/swtp6800/common/mp-b2.c` | 16 |
| `simulators/3B2/3b2_rev3_mmu.c` | 15 |
| `simulators/3B2/3b2_scsi.c` | 15 |
| `simulators/3B2/3b2_timer.c` | 15 |
| `simulators/B5500/b5500_urec.c` | 15 |
| `simulators/HP2100/hp2100_pif.c` | 15 |
| `simulators/I7000/i7010_sys.c` | 15 |
| `simulators/I7000/i7070_sys.c` | 15 |
| `simulators/I7000/i7080_sys.c` | 15 |
| `simulators/Intel-Systems/common/i8259.c` | 15 |
| `simulators/PDP11/pdp11_lp.c` | 15 |
| `simulators/PDP8/pdp8_clk.c` | 15 |
| `simulators/VAX/vax630_io.c` | 15 |
| `simulators/VAX/vax730_rb.c` | 15 |
| `simulators/VAX/vax_io.c` | 15 |
| `simulators/alpha/alpha_ev5_tlb.c` | 15 |
| `simulators/swtp6800/common/m6800.c` | 15 |
| `simulators/AltairZ80/s100_fif.c` | 14 |
| `simulators/CDC1700/cdc1700_lp.c` | 14 |
| `simulators/Intel-Systems/common/i3214.c` | 14 |
| `simulators/NOVA/nova_plt.c` | 14 |
| `simulators/PDP18B/pdp18b_rb.c` | 14 |
| `simulators/PDP8/pdp8_lp.c` | 14 |
| `simulators/SDS/sds_dsk.c` | 14 |
| `simulators/VAX/vax420_sysdev.c` | 14 |
| `simulators/tt2500/tt2500_dpy.c` | 14 |
| `simulators/AltairZ80/s100_64fdc.c` | 13 |
| `simulators/AltairZ80/s100_if3.c` | 13 |
| `simulators/Ibm1130/ibm1130_gdu.c` | 13 |
| `simulators/Intel-Systems/scelbi/scelbi_io.c` | 13 |
| `simulators/PDP10/ka10_ch10.c` | 13 |
| `simulators/PDP11/pdp11_ch.c` | 13 |
| `simulators/PDP11/pdp11_mb.c` | 13 |
| `simulators/PDP18B/pdp18b_drm.c` | 13 |
| `simulators/SAGE/i8272.c` | 13 |
| `simulators/SDS/sds_drm.c` | 13 |
| `simulators/SDS/sds_rad.c` | 13 |
| `simulators/VAX/vax820_mem.c` | 13 |
| `simulators/VAX/vax860_sbia.c` | 13 |
| `simulators/VAX/vax_va.c` | 13 |
| `simulators/linc/linc_tape.c` | 13 |
| `simulators/3B2/3b2_stddev.c` | 12 |
| `simulators/AltairZ80/n8vem.c` | 12 |
| `simulators/AltairZ80/s100_mdriveh.c` | 12 |
| `simulators/B5500/b5500_dtc.c` | 12 |
| `simulators/CDC1700/cdc1700_dp.c` | 12 |
| `simulators/I1620/i1620_dp.c` | 12 |
| `simulators/I1620/i1620_pt.c` | 12 |
| `simulators/I7000/i7000_ht.c` | 12 |
| `simulators/Ibm1130/ibm1130_prt.c` | 12 |
| `simulators/ND100/nd100_floppy.c` | 12 |
| `simulators/PDP10/ks10_ch11.c` | 12 |
| `simulators/PDP10/kx10_dc.c` | 12 |
| `simulators/PDP10/kx10_pt.c` | 12 |
| `simulators/PDP10/kx10_tym.c` | 12 |
| `simulators/PDP10/pdp10_fe.c` | 12 |
| `simulators/PDP10/pdp6_dcs.c` | 12 |
| `simulators/S3/s3_pkb.c` | 12 |
| `simulators/SAGE/sage_cpu.c` | 12 |
| `simulators/VAX/vax410_sysdev.c` | 12 |
| `simulators/VAX/vax440_sysdev.c` | 12 |
| `simulators/VAX/vax4xx_rz94.c` | 12 |
| `simulators/VAX/vax860_abus.c` | 12 |
| `simulators/3B2/3b2_mau.c` | 11 |
| `simulators/ALTAIR/altair_dsk.c` | 11 |
| `simulators/HP2100/hp2100_di.c` | 11 |
| `simulators/HP3000/hp3000_iop.c` | 11 |
| `simulators/Ibm1130/ibm1130_ptrp.c` | 11 |
| `simulators/NOVA/nova_lp.c` | 11 |
| `simulators/PDP10/ka10_pmp.c` | 11 |
| `simulators/PDP11/pdp11_kg.c` | 11 |
| `simulators/PDP8/pdp8_tsc.c` | 11 |
| `simulators/VAX/vax750_cmi.c` | 11 |
| `simulators/VAX/vax750_mem.c` | 11 |
| `simulators/VAX/vax820_bi.c` | 11 |
| `simulators/sigma/sigma_map.c` | 11 |
| `simulators/swtp6800/common/mp-s.c` | 11 |
| `simulators/AltairZ80/altairz80_mhdsk.c` | 10 |
| `simulators/CDC1700/cdc1700_drm.c` | 10 |
| `simulators/CDC1700/cdc1700_rtc.c` | 10 |
| `simulators/Intel-Systems/common/ioc-cont.c` | 10 |
| `simulators/Intel-Systems/common/ipc-cont.c` | 10 |
| `simulators/Intel-Systems/common/irq.c` | 10 |
| `simulators/Intel-Systems/common/multibus.c` | 10 |
| `simulators/PDP10/ks10_cty.c` | 10 |
| `simulators/PDP10/ks10_dz.c` | 10 |
| `simulators/PDP10/kx10_cty.c` | 10 |
| `simulators/SAGE/sage_fd.c` | 10 |
| `simulators/SEL32/sel32_disk.c` | 10 |
| `simulators/SEL32/sel32_hsdp.c` | 10 |
| `simulators/SEL32/sel32_scfi.c` | 10 |
| `simulators/VAX/vax43_sysdev.c` | 10 |
| `simulators/VAX/vax730_mem.c` | 10 |
| `simulators/VAX/vax820_ka.c` | 10 |
| `simulators/swtp6800/common/dc-4.c` | 10 |
| `simulators/tt2500/tt2500_uart.c` | 10 |
| `simulators/AltairZ80/s100_disk1a.c` | 9 |
| `simulators/AltairZ80/vfdhd.c` | 9 |
| `simulators/H316/h316_imp.c` | 9 |
| `simulators/I1401/i1401_iq.c` | 9 |
| `simulators/I1620/i1620_cd.c` | 9 |
| `simulators/I1620/i1620_tty.c` | 9 |
| `simulators/Intel-Systems/common/ieprom.c` | 9 |
| `simulators/Intel-Systems/common/sys.c` | 9 |
| `simulators/PDP10/kx10_lp.c` | 9 |
| `simulators/SDS/sds_cp.c` | 9 |
| `simulators/SEL32/sel32_mt.c` | 9 |
| `simulators/VAX/vax4xx_va.c` | 9 |
| `simulators/VAX/vax610_sysdev.c` | 9 |
| `simulators/imlac/imlac_tty.c` | 9 |
| `simulators/swtp6800/common/bootrom.c` | 9 |
| `simulators/3B2/3b2_id.c` | 8 |
| `simulators/3B2/3b2_ports.c` | 8 |
| `simulators/AltairZ80/mfdc.c` | 8 |
| `simulators/AltairZ80/s100_jadedd.c` | 8 |
| `simulators/AltairZ80/s100_mdsa.c` | 8 |
| `simulators/AltairZ80/s100_mdsad.c` | 8 |
| `simulators/BESM6/besm6_drum.c` | 8 |
| `simulators/I7000/i701_sys.c` | 8 |
| `simulators/Ibm1130/ibm1130_t2741.c` | 8 |
| `simulators/Intel-Systems/common/iram8.c` | 8 |
| `simulators/Intel-Systems/common/isbc064.c` | 8 |
| `simulators/Intel-Systems/common/isbc464.c` | 8 |
| `simulators/PDP1/pdp1_clk.c` | 8 |
| `simulators/PDP10/pdp6_dsk.c` | 8 |
| `simulators/PDP10/pdp6_ge.c` | 8 |
| `simulators/PDP11/pdp11_ke.c` | 8 |
| `simulators/SDS/sds_cr.c` | 8 |
| `simulators/SSEM/ssem_cpu.c` | 8 |
| `simulators/VAX/vax610_io.c` | 8 |
| `simulators/swtp6800/common/fd400.c` | 8 |
| `simulators/swtp6800/common/mp-8m.c` | 8 |
| `simulators/swtp6800/common/mp-a.c` | 8 |
| `simulators/3B2/3b2_dmac.c` | 7 |
| `simulators/AltairZ80/i8272.c` | 7 |
| `simulators/AltairZ80/s100_selchan.c` | 7 |
| `simulators/AltairZ80/s100_tdd.c` | 7 |
| `simulators/I650/i650_mt.c` | 7 |
| `simulators/I7000/i7090_cdp.c` | 7 |
| `simulators/I7000/i7090_cdr.c` | 7 |
| `simulators/ND100/nd100_mm.c` | 7 |
| `simulators/PDP10/kl10_nia.c` | 7 |
| `simulators/PDP10/kx10_imp.c` | 7 |
| `simulators/PDQ-3/pdq3_sys.c` | 7 |
| `simulators/SAGE/m68k_scp.c` | 7 |
| `simulators/SEL32/sel32_scsi.c` | 7 |
| `simulators/imlac/imlac_pt.c` | 7 |
| `simulators/sigma/sigma_cp.c` | 7 |
| `simulators/sigma/sigma_cr.c` | 7 |
| `simulators/AltairZ80/s100_dazzler.c` | 6 |
| `simulators/AltairZ80/s100_vdm1.c` | 6 |
| `simulators/BESM6/besm6_punch.c` | 6 |
| `simulators/BESM6/besm6_vu.c` | 6 |
| `simulators/I7000/i7090_lpr.c` | 6 |
| `simulators/PDP10/ka10_auxcpu.c` | 6 |
| `simulators/PDP10/ka10_ten11.c` | 6 |
| `simulators/PDP10/kx10_ddc.c` | 6 |
| `simulators/PDP10/pdp6_dct.c` | 6 |
| `simulators/PDP10/pdp6_slave.c` | 6 |
| `simulators/VAX/vax610_mem.c` | 6 |
| `simulators/VAX/vax730_sys.c` | 6 |
| `simulators/VAX/vax_uw.c` | 6 |
| `simulators/imlac/imlac_kbd.c` | 6 |
| `simulators/linc/linc_crt.c` | 6 |
| `simulators/swtp6800/common/i2716.c` | 6 |
| `simulators/tt2500/tt2500_key.c` | 6 |
| `src/runtime/sim_tape.c` | 6 |
| `src/runtime/sim_tmxr.c` | 6 |
| `simulators/3B2/3b2_ni.c` | 5 |
| `simulators/3B2/3b2_rev2_csr.c` | 5 |
| `simulators/3B2/3b2_rev3_csr.c` | 5 |
| `simulators/AltairZ80/altairz80_net.c` | 5 |
| `simulators/AltairZ80/flashwriter2.c` | 5 |
| `simulators/BESM6/besm6_printer.c` | 5 |
| `simulators/I650/i650_cdp.c` | 5 |
| `simulators/I650/i650_cdr.c` | 5 |
| `simulators/I650/i650_dsk.c` | 5 |
| `simulators/I7000/i701_chan.c` | 5 |
| `simulators/I7094/i7094_clk.c` | 5 |
| `simulators/PDP10/ka10_dpk.c` | 5 |
| `simulators/PDP10/ka10_imx.c` | 5 |
| `simulators/PDP10/ka10_mty.c` | 5 |
| `simulators/PDP10/ka10_pclk.c` | 5 |
| `simulators/PDP10/ka10_pd.c` | 5 |
| `simulators/PDP10/kx10_cp.c` | 5 |
| `simulators/PDP10/kx10_cr.c` | 5 |
| `simulators/PDP10/pdp10_pag.c` | 5 |
| `simulators/SAGE/i8259.c` | 5 |
| `simulators/VAX/vax_xs.h` | 5 |
| `simulators/imlac/imlac_crt.c` | 5 |
| `simulators/linc/linc_tty.c` | 5 |
| `simulators/tt2500/tt2500_crt.c` | 5 |
| `simulators/tt2500/tt2500_tv.c` | 5 |
| `simulators/3B2/3b2_ctc.c` | 4 |
| `simulators/BESM6/besm6_punchcard.c` | 4 |
| `simulators/HP3000/hp3000_cpu_base.c` | 4 |
| `simulators/I7000/i7090_drum.c` | 4 |
| `simulators/I7000/i7090_hdrum.c` | 4 |
| `simulators/Ibm1130/ibm1130_gui.c` | 4 |
| `simulators/PDP10/ka10_tk10.c` | 4 |
| `simulators/PDP10/ks10_tcu.c` | 4 |
| `simulators/PDP10/kx10_dk.c` | 4 |
| `simulators/SAGE/i8251.c` | 4 |
| `simulators/SEL32/sel32_con.c` | 4 |
| `simulators/VAX/vax4xx_ve.c` | 4 |
| `simulators/VAX/vax_mmu.c` | 4 |
| `simulators/3B2/3b2_if.c` | 3 |
| `simulators/B5500/b5500_dr.c` | 3 |
| `simulators/BESM6/besm6_pl.c` | 3 |
| `simulators/SAGE/i8253.c` | 3 |
| `simulators/SEL32/sel32_lpr.c` | 3 |
| `simulators/VAX/vax_lk.c` | 3 |
| `simulators/VAX/vax_nar.c` | 3 |
| `simulators/VAX/vax_vs.c` | 3 |
| `simulators/alpha/alpha_io.c` | 3 |
| `simulators/imlac/imlac_bel.c` | 3 |
| `simulators/linc/linc_dpy.c` | 3 |
| `simulators/linc/linc_kbd.c` | 3 |
| `simulators/swtp6800/common/m6810.c` | 3 |
| `simulators/CDC1700/cdc1700_sys.c` | 2 |
| `simulators/I7000/i7000_cdr.c` | 2 |
| `simulators/I7000/i7000_chron.c` | 2 |
| `simulators/I7000/i7000_con.c` | 2 |
| `simulators/I7000/i7000_lpr.c` | 2 |
| `simulators/I7000/i7080_drum.c` | 2 |
| `simulators/PDP10/kx10_sys.c` | 2 |
| `simulators/PDP11/pdp11_xu.h` | 2 |
| `simulators/SEL32/sel32_iop.c` | 2 |
| `simulators/SEL32/sel32_mfp.c` | 2 |
| `simulators/AltairZ80/disasm.c` | 1 |
| `simulators/B5500/b5500_sys.c` | 1 |
| `simulators/H316/h316_udp.c` | 1 |
| `simulators/Ibm1130/ibm1130_plot.c` | 1 |
| `simulators/Interdata/id16_dboot.c` | 1 |
| `simulators/Interdata/id32_dboot.c` | 1 |
| `simulators/PDP11/pdp11_rom.c` | 1 |
| `simulators/PDP11/pdp11_td.h` | 1 |
| `simulators/SEL32/sel32_sys.c` | 1 |
| `src/runtime/sim_disk.c` | 1 |

### `-Wunused-parameter` (6454 unique sites)

Example sites:

- `simulators/3B2/3b2_cpu.c:812`: unused parameter 'uptr'
- `simulators/3B2/3b2_cpu.c:812`: unused parameter 'val'
- `simulators/3B2/3b2_cpu.c:847`: unused parameter 'desc'
- `simulators/3B2/3b2_cpu.c:847`: unused parameter 'uptr'
- `simulators/3B2/3b2_cpu.c:847`: unused parameter 'val'

| File | Unique sites |
|---|---:|
| `src/core/scp.c` | 115 |
| `simulators/PDP10/kx10_tym.c` | 100 |
| `src/runtime/sim_console.c` | 85 |
| `simulators/AltairZ80/altairz80_cpu.c` | 75 |
| `src/runtime/sim_video.c` | 75 |
| `simulators/VAX/vax630_sysdev.c` | 54 |
| `simulators/VAX/vax_sysdev.c` | 51 |
| `simulators/HP2100/hp2100_cpu.c` | 44 |
| `simulators/H316/h316_cpu.c` | 43 |
| `simulators/GRI/gri_cpu.c` | 42 |
| `simulators/PDP11/pdp11_cpumod.c` | 42 |
| `simulators/AltairZ80/altairz80_sio.c` | 41 |
| `simulators/PDP11/pdp11_xq.c` | 40 |
| `simulators/I1620/i1620_cpu.c` | 39 |
| `simulators/PDP10/kl10_fe.c` | 39 |
| `simulators/PDP11/pdp11_dmc.c` | 39 |
| `simulators/VAX/vax4xx_stddev.c` | 38 |
| `simulators/PDP10/kx10_imp.c` | 37 |
| `simulators/PDP11/pdp11_cr.c` | 37 |
| `simulators/PDP10/kx10_cpu.c` | 34 |
| `simulators/AltairZ80/sol20.c` | 33 |
| `simulators/Intel-Systems/common/isbc208.c` | 33 |
| `src/runtime/sim_scsi.c` | 33 |
| `simulators/ND100/nd100_cpu.c` | 32 |
| `simulators/VAX/vax4nn_stddev.c` | 32 |
| `src/runtime/sim_timer.c` | 32 |
| `simulators/PDP11/pdp11_rl.c` | 31 |
| `simulators/VAX/vax43_sysdev.c` | 31 |
| `simulators/sigma/sigma_cpu.c` | 31 |
| `simulators/AltairZ80/s100_dazzler.c` | 30 |
| `simulators/PDP11/pdp11_xu.c` | 30 |
| `simulators/VAX/vax_va.c` | 30 |
| `src/runtime/sim_tmxr.c` | 30 |
| `simulators/SAGE/m68k_sys.c` | 29 |
| `simulators/SEL32/sel32_cpu.c` | 29 |
| `simulators/VAX/is1000_sysdev.c` | 29 |
| `simulators/VAX/vax440_sysdev.c` | 29 |
| `simulators/HP3000/hp3000_cpu.c` | 28 |
| `simulators/VAX/vax4xx_va.c` | 28 |
| `simulators/AltairZ80/s100_dj2d.c` | 27 |
| `simulators/B5500/b5500_urec.c` | 27 |
| `simulators/PDP10/ks10_dup.c` | 27 |
| `simulators/PDP11/pdp11_dup.c` | 27 |
| `simulators/3B2/3b2_cpu.c` | 26 |
| `simulators/AltairZ80/s100_vdm1.c` | 26 |
| `simulators/PDP10/pdp10_lp20.c` | 26 |
| `simulators/PDP11/pdp11_rq.c` | 26 |
| `simulators/VAX/vax420_sysdev.c` | 26 |
| `simulators/AltairZ80/s100_jair.c` | 25 |
| `simulators/AltairZ80/s100_tarbell.c` | 25 |
| `simulators/PDP18B/pdp18b_cpu.c` | 24 |
| `simulators/PDP18B/pdp18b_stddev.c` | 24 |
| `simulators/SEL32/sel32_ipu.c` | 24 |
| `simulators/TX-0/tx0_cpu.c` | 24 |
| `simulators/B5500/b5500_dtc.c` | 23 |
| `simulators/BESM6/besm6_vu.c` | 23 |
| `simulators/CDC1700/cdc1700_mt.c` | 23 |
| `simulators/Ibm1130/ibm1130_sys.c` | 23 |
| `simulators/Intel-Systems/common/zx200a.c` | 23 |
| `simulators/PDP10/pdp10_ksio.c` | 23 |
| `simulators/PDP11/pdp11_vh.c` | 23 |
| `simulators/SDS/sds_cpu.c` | 23 |
| `simulators/sigma/sigma_io.c` | 23 |
| `simulators/3B2/3b2_stddev.c` | 22 |
| `simulators/CDC1700/cdc1700_dev1.c` | 22 |
| `simulators/LGP/lgp_cpu.c` | 22 |
| `simulators/VAX/vax730_stddev.c` | 22 |
| `simulators/VAX/vax750_stddev.c` | 22 |
| `src/runtime/sim_disk.c` | 22 |
| `simulators/H316/h316_stddev.c` | 21 |
| `simulators/NOVA/eclipse_cpu.c` | 21 |
| `simulators/SAGE/sage_stddev.c` | 21 |
| `simulators/SEL32/sel32_clk.c` | 21 |
| `simulators/VAX/vax820_stddev.c` | 21 |
| `simulators/VAX/vax860_abus.c` | 21 |
| `simulators/VAX/vax_cpu.c` | 21 |
| `simulators/imlac/imlac_cpu.c` | 21 |
| `src/runtime/sim_tape.c` | 21 |
| `simulators/AltairZ80/s100_icom.c` | 20 |
| `simulators/H316/h316_rtc.c` | 20 |
| `simulators/Intel-Systems/common/isbc201.c` | 20 |
| `simulators/Intel-Systems/common/isbc202.c` | 20 |
| `simulators/Intel-Systems/common/isbc206.c` | 20 |
| `simulators/PDP11/pdp11_cpu.c` | 20 |
| `simulators/VAX/vax410_sysdev.c` | 20 |
| `simulators/VAX/vax750_cmi.c` | 20 |
| `simulators/VAX/vax780_stddev.c` | 20 |
| `simulators/VAX/vax860_stddev.c` | 20 |
| `simulators/B5500/b5500_cpu.c` | 19 |
| `simulators/CDC1700/cdc1700_cd.c` | 19 |
| `simulators/CDC1700/cdc1700_dp.c` | 19 |
| `simulators/I7000/i7090_cpu.c` | 19 |
| `simulators/PDP1/pdp1_cpu.c` | 19 |
| `simulators/PDP10/pdp10_cpu.c` | 19 |
| `simulators/PDP11/pdp11_tq.c` | 19 |
| `simulators/BESM6/besm6_tty.c` | 18 |
| `simulators/HP2100/hp2100_lps.c` | 18 |
| `simulators/I1401/i1401_cpu.c` | 18 |
| `simulators/I7000/i7070_cpu.c` | 18 |
| `simulators/Ibm1130/ibm1130_cpu.c` | 18 |
| `simulators/Interdata/id_io.c` | 18 |
| `simulators/PDP11/pdp11_rom.c` | 18 |
| `simulators/PDP11/pdp11_stddev.c` | 18 |
| `simulators/VAX/vax630_io.c` | 18 |
| `simulators/VAX/vax_io.c` | 18 |
| `simulators/swtp6800/common/m6800.c` | 18 |
| `simulators/BESM6/besm6_cpu.c` | 17 |
| `simulators/HP3000/hp3000_lp.c` | 17 |
| `simulators/I7000/i7010_cpu.c` | 17 |
| `simulators/PDP10/kx10_dc.c` | 17 |
| `simulators/PDP11/pdp11_ch.c` | 17 |
| `simulators/PDP11/pdp11_io_lib.c` | 17 |
| `simulators/PDP8/pdp8_cpu.c` | 17 |
| `simulators/VAX/vax610_sysdev.c` | 17 |
| `simulators/VAX/vax780_sbi.c` | 17 |
| `simulators/I1620/i1620_pt.c` | 16 |
| `simulators/I7000/i7000_dsk.c` | 16 |
| `simulators/I7000/i7010_chan.c` | 16 |
| `simulators/I7000/i7080_cpu.c` | 16 |
| `simulators/Ibm1130/ibm1130_stddev.c` | 16 |
| `simulators/Interdata/id16_cpu.c` | 16 |
| `simulators/LGP/lgp_stddev.c` | 16 |
| `simulators/PDP1/pdp1_stddev.c` | 16 |
| `simulators/PDP10/ka10_ch10.c` | 16 |
| `simulators/PDP10/ks10_ch11.c` | 16 |
| `simulators/PDP10/ks10_uba.c` | 16 |
| `simulators/PDP11/pdp11_hk.c` | 16 |
| `simulators/S3/s3_cpu.c` | 16 |
| `simulators/S3/s3_disk.c` | 16 |
| `simulators/VAX/vax820_bi.c` | 16 |
| `simulators/VAX/vax_stddev.c` | 16 |
| `simulators/CDC1700/cdc1700_cpu.c` | 15 |
| `simulators/GRI/gri_stddev.c` | 15 |
| `simulators/HP2100/hp2100_tty.c` | 15 |
| `simulators/Interdata/id32_cpu.c` | 15 |
| `simulators/NOVA/nova_cpu.c` | 15 |
| `simulators/PDP11/pdp11_rr.c` | 15 |
| `simulators/PDQ-3/pdq3_sys.c` | 15 |
| `simulators/VAX/vax610_stddev.c` | 15 |
| `simulators/VAX/vax820_uba.c` | 15 |
| `simulators/VAX/vax_nar.c` | 15 |
| `simulators/3B2/3b2_id.c` | 14 |
| `simulators/3B2/3b2_iu.c` | 14 |
| `simulators/AltairZ80/mmd.c` | 14 |
| `simulators/AltairZ80/s100_adcs6.c` | 14 |
| `simulators/HP2100/hp2100_ipl.c` | 14 |
| `simulators/HP2100/hp2100_lpt.c` | 14 |
| `simulators/HP2100/hp2100_ms.c` | 14 |
| `simulators/I7000/i7000_chan.c` | 14 |
| `simulators/I7000/i701_cpu.c` | 14 |
| `simulators/I7000/i7070_chan.c` | 14 |
| `simulators/I7000/i7080_chan.c` | 14 |
| `simulators/Ibm1130/ibm1130_cr.c` | 14 |
| `simulators/Intel-Systems/common/i8080.c` | 14 |
| `simulators/PDP11/pdp11_pt.c` | 14 |
| `simulators/PDP11/pdp11_td.c` | 14 |
| `simulators/PDQ-3/pdq3_mem.c` | 14 |
| `simulators/SEL32/sel32_ec.c` | 14 |
| `simulators/TX-0/tx0_stddev.c` | 14 |
| `simulators/VAX/vax4xx_ve.c` | 14 |
| `simulators/VAX/vax730_rb.c` | 14 |
| `simulators/VAX/vax730_sys.c` | 14 |
| `simulators/BESM6/besm6_panel.c` | 13 |
| `simulators/CDC1700/cdc1700_io.c` | 13 |
| `simulators/HP3000/hp3000_sys.c` | 13 |
| `simulators/I7000/i7000_com.c` | 13 |
| `simulators/Ibm1130/ibm1130_gui.c` | 13 |
| `simulators/Intel-Systems/common/i3214.c` | 13 |
| `simulators/PDP10/ks10_cty.c` | 13 |
| `simulators/PDP10/kx10_cty.c` | 13 |
| `simulators/PDP10/kx10_lp.c` | 13 |
| `simulators/VAX/vax780_uba.c` | 13 |
| `simulators/VAX/vax_vc.c` | 13 |
| `simulators/alpha/alpha_cpu.c` | 13 |
| `simulators/3B2/3b2_if.c` | 12 |
| `simulators/3B2/3b2_rev3_mmu.c` | 12 |
| `simulators/B5500/b5500_dr.c` | 12 |
| `simulators/I650/i650_cpu.c` | 12 |
| `simulators/I7094/i7094_cpu.c` | 12 |
| `simulators/PDP10/ka10_imx.c` | 12 |
| `simulators/PDP10/ka10_pclk.c` | 12 |
| `simulators/PDP10/ka10_pd.c` | 12 |
| `simulators/PDP10/ka10_pmp.c` | 12 |
| `simulators/PDP10/kx10_rh.c` | 12 |
| `simulators/PDP11/pdp11_kmc.c` | 12 |
| `simulators/PDP11/pdp11_rk.c` | 12 |
| `simulators/PDQ-3/pdq3_cpu.c` | 12 |
| `simulators/SEL32/sel32_disk.c` | 12 |
| `simulators/SEL32/sel32_hsdp.c` | 12 |
| `simulators/SEL32/sel32_scfi.c` | 12 |
| `simulators/VAX/vax4xx_vc.c` | 12 |
| `simulators/VAX/vax630_stddev.c` | 12 |
| `simulators/VAX/vax750_uba.c` | 12 |
| `simulators/VAX/vax_watch.c` | 12 |
| `simulators/linc/linc_cpu.c` | 12 |
| `simulators/3B2/3b2_ports.c` | 11 |
| `simulators/3B2/3b2_rev2_csr.c` | 11 |
| `simulators/3B2/3b2_rev3_csr.c` | 11 |
| `simulators/B5500/b5500_dk.c` | 11 |
| `simulators/H316/h316_dp.c` | 11 |
| `simulators/HP2100/hp2100_dr.c` | 11 |
| `simulators/HP3000/hp_disclib.c` | 11 |
| `simulators/Intel-Systems/common/iram8.c` | 11 |
| `simulators/Intel-Systems/common/isbc064.c` | 11 |
| `simulators/Interdata/id_tt.c` | 11 |
| `simulators/PDP10/ks10_dz.c` | 11 |
| `simulators/PDP10/pdp10_pag.c` | 11 |
| `simulators/PDP10/pdp6_dcs.c` | 11 |
| `simulators/PDP10/pdp6_dsk.c` | 11 |
| `simulators/PDP11/pdp11_dl.c` | 11 |
| `simulators/PDP11/pdp11_dz.c` | 11 |
| `simulators/PDP11/pdp11_tu.c` | 11 |
| `simulators/PDP18B/pdp18b_lp.c` | 11 |
| `simulators/SEL32/sel32_scsi.c` | 11 |
| `simulators/VAX/vax4xx_dz.c` | 11 |
| `simulators/VAX/vax730_uba.c` | 11 |
| `simulators/VAX/vax820_mem.c` | 11 |
| `simulators/AltairZ80/s100_jadedd.c` | 10 |
| `simulators/CDC1700/cdc1700_rtc.c` | 10 |
| `simulators/I1401/i1401_cd.c` | 10 |
| `simulators/I650/i650_cdp.c` | 10 |
| `simulators/I650/i650_mt.c` | 10 |
| `simulators/I7000/i7000_chron.c` | 10 |
| `simulators/I7000/i7090_drum.c` | 10 |
| `simulators/I7000/i7090_hdrum.c` | 10 |
| `simulators/I7000/i7090_lpr.c` | 10 |
| `simulators/I7094/i7094_cd.c` | 10 |
| `simulators/Intel-Systems/common/i8008.c` | 10 |
| `simulators/Interdata/id_ttp.c` | 10 |
| `simulators/Interdata/id_uvc.c` | 10 |
| `simulators/PDP10/ks10_kmc.c` | 10 |
| `simulators/PDP10/kx10_pt.c` | 10 |
| `simulators/PDP11/pdp11_rf.c` | 10 |
| `simulators/PDP11/pdp11_ry.c` | 10 |
| `simulators/PDP11/pdp11_tc.c` | 10 |
| `simulators/SEL32/sel32_lpr.c` | 10 |
| `simulators/sigma/sigma_lp.c` | 10 |
| `simulators/swtp6800/common/mp-s.c` | 10 |
| `simulators/tt2500/tt2500_sys.c` | 10 |
| `simulators/ALTAIR/altair_cpu.c` | 9 |
| `simulators/AltairZ80/altairz80_hdsk.c` | 9 |
| `simulators/AltairZ80/altairz80_sys.c` | 9 |
| `simulators/HP2100/hp2100_dp.c` | 9 |
| `simulators/I1620/i1620_tty.c` | 9 |
| `simulators/I650/i650_sys.c` | 9 |
| `simulators/I7000/i7000_cdp.c` | 9 |
| `simulators/I7000/i7000_ht.c` | 9 |
| `simulators/I7000/i7000_lpr.c` | 9 |
| `simulators/I7000/i701_chan.c` | 9 |
| `simulators/Intel-Systems/common/sys.c` | 9 |
| `simulators/ND100/nd100_sys.c` | 9 |
| `simulators/NOVA/nova_tt.c` | 9 |
| `simulators/PDP10/ks10_lp.c` | 9 |
| `simulators/PDP10/ks10_tcu.c` | 9 |
| `simulators/PDP10/kx10_mt.c` | 9 |
| `simulators/PDP11/pdp11_pclk.c` | 9 |
| `simulators/PDP11/pdp11_rp.c` | 9 |
| `simulators/PDP11/pdp11_uc15.c` | 9 |
| `simulators/PDP18B/pdp18b_dr15.c` | 9 |
| `simulators/PDP8/pdp8_rx.c` | 9 |
| `simulators/SDS/sds_stddev.c` | 9 |
| `simulators/VAX/vax610_mem.c` | 9 |
| `simulators/VAX/vax730_mem.c` | 9 |
| `simulators/imlac/imlac_tty.c` | 9 |
| `simulators/sigma/sigma_dp.c` | 9 |
| `simulators/tt2500/tt2500_cpu.c` | 9 |
| `third_party/slirp/slirp.c` | 9 |
| `simulators/AltairZ80/altairz80_mhdsk.c` | 8 |
| `simulators/B5500/b5500_sys.c` | 8 |
| `simulators/CDC1700/cdc1700_drm.c` | 8 |
| `simulators/CDC1700/cdc1700_lp.c` | 8 |
| `simulators/H316/h316_imp.c` | 8 |
| `simulators/HP2100/hp2100_mpx.c` | 8 |
| `simulators/HP3000/hp3000_atc.c` | 8 |
| `simulators/HP3000/hp_tapelib.c` | 8 |
| `simulators/I1620/i1620_cd.c` | 8 |
| `simulators/I650/i650_cdr.c` | 8 |
| `simulators/I7000/i7000_mt.c` | 8 |
| `simulators/I7000/i7070_sys.c` | 8 |
| `simulators/I7094/i7094_com.c` | 8 |
| `simulators/I7094/i7094_io.c` | 8 |
| `simulators/Intel-Systems/common/ioc-cont.c` | 8 |
| `simulators/Intel-Systems/common/isbc464.c` | 8 |
| `simulators/Intel-Systems/scelbi/scelbi_io.c` | 8 |
| `simulators/NOVA/eclipse_tt.c` | 8 |
| `simulators/PDP8/pdp8_pt.c` | 8 |
| `simulators/PDP8/pdp8_rl.c` | 8 |
| `simulators/S3/s3_cd.c` | 8 |
| `simulators/SEL32/sel32_iop.c` | 8 |
| `simulators/SEL32/sel32_mfp.c` | 8 |
| `simulators/sigma/sigma_rad.c` | 8 |
| `simulators/sigma/sigma_rtc.c` | 8 |
| `simulators/3B2/3b2_timer.c` | 7 |
| `simulators/AltairZ80/altairz80_net.c` | 7 |
| `simulators/AltairZ80/s100_64fdc.c` | 7 |
| `simulators/AltairZ80/s100_fif.c` | 7 |
| `simulators/BESM6/besm6_disk.c` | 7 |
| `simulators/CDC1700/cdc1700_dc.c` | 7 |
| `simulators/HP2100/hp2100_pif.c` | 7 |
| `simulators/I7000/i7000_con.c` | 7 |
| `simulators/I7000/i7010_sys.c` | 7 |
| `simulators/I7000/i7090_chan.c` | 7 |
| `simulators/Ibm1130/ibm1130_sca.c` | 7 |
| `simulators/Intel-Systems/common/ipc-cont.c` | 7 |
| `simulators/Intel-Systems/common/port.c` | 7 |
| `simulators/NOVA/nova_qty.c` | 7 |
| `simulators/PDP10/ka10_auxcpu.c` | 7 |
| `simulators/PDP10/ka10_ten11.c` | 7 |
| `simulators/PDP10/kl10_nia.c` | 7 |
| `simulators/PDP10/kx10_disk.c` | 7 |
| `simulators/PDP10/kx10_dp.c` | 7 |
| `simulators/PDP10/kx10_rs.c` | 7 |
| `simulators/PDP10/pdp10_rp.c` | 7 |
| `simulators/PDP10/pdp6_mtc.c` | 7 |
| `simulators/PDP11/pdp11_kg.c` | 7 |
| `simulators/PDP11/pdp11_lp.c` | 7 |
| `simulators/PDP11/pdp11_rc.c` | 7 |
| `simulators/PDP11/pdp11_rs.c` | 7 |
| `simulators/PDP8/pdp8_clk.c` | 7 |
| `simulators/PDP8/pdp8_rf.c` | 7 |
| `simulators/PDP8/pdp8_ttx.c` | 7 |
| `simulators/SAGE/sage_fd.c` | 7 |
| `simulators/SDS/sds_io.c` | 7 |
| `simulators/VAX/vax4xx_rd.c` | 7 |
| `simulators/VAX/vax750_mem.c` | 7 |
| `simulators/imlac/imlac_kbd.c` | 7 |
| `simulators/imlac/imlac_sys.c` | 7 |
| `src/runtime/sim_card.c` | 7 |
| `simulators/ALTAIR/altair_sio.c` | 6 |
| `simulators/AltairZ80/s100_scp300f.c` | 6 |
| `simulators/CDC1700/cdc1700_sys.c` | 6 |
| `simulators/GRI/gri_sys.c` | 6 |
| `simulators/HP2100/hp2100_mem.c` | 6 |
| `simulators/HP3000/hp3000_iop.c` | 6 |
| `simulators/I650/i650_dsk.c` | 6 |
| `simulators/I7000/i701_sys.c` | 6 |
| `simulators/I7000/i7080_drum.c` | 6 |
| `simulators/I7000/i7080_sys.c` | 6 |
| `simulators/Ibm1130/ibm1130_disk.c` | 6 |
| `simulators/LGP/lgp_sys.c` | 6 |
| `simulators/NOVA/nova_clk.c` | 6 |
| `simulators/PDP1/pdp1_lp.c` | 6 |
| `simulators/PDP10/kx10_dt.c` | 6 |
| `simulators/PDP10/kx10_rc.c` | 6 |
| `simulators/PDP10/kx10_sys.c` | 6 |
| `simulators/PDP10/pdp10_fe.c` | 6 |
| `simulators/PDP10/pdp10_sys.c` | 6 |
| `simulators/PDP10/pdp6_dtc.c` | 6 |
| `simulators/PDP11/pdp11_ke.c` | 6 |
| `simulators/PDP11/pdp11_rh.c` | 6 |
| `simulators/PDP11/pdp11_ta.c` | 6 |
| `simulators/PDP11/pdp11_tm.c` | 6 |
| `simulators/PDP11/pdp11_ts.c` | 6 |
| `simulators/PDP18B/pdp18b_drm.c` | 6 |
| `simulators/PDP8/pdp8_df.c` | 6 |
| `simulators/PDP8/pdp8_td.c` | 6 |
| `simulators/PDP8/pdp8_tt.c` | 6 |
| `simulators/SAGE/sage_cpu.c` | 6 |
| `simulators/SDS/sds_mux.c` | 6 |
| `simulators/SSEM/ssem_cpu.c` | 6 |
| `simulators/VAX/vax7x0_mba.c` | 6 |
| `simulators/VAX/vax820_ka.c` | 6 |
| `simulators/VAX/vax860_sbia.c` | 6 |
| `src/runtime/sim_ether.c` | 6 |
| `third_party/slirp_glue/sim_slirp.c` | 6 |
| `simulators/3B2/3b2_rev2_mmu.c` | 5 |
| `simulators/3B2/3b2_scsi.c` | 5 |
| `simulators/AltairZ80/altairz80_dsk.c` | 5 |
| `simulators/AltairZ80/i86_decode.c` | 5 |
| `simulators/AltairZ80/ibc.c` | 5 |
| `simulators/AltairZ80/s100_2sio.c` | 5 |
| `simulators/AltairZ80/s100_djhdc.c` | 5 |
| `simulators/AltairZ80/s100_hdc1001.c` | 5 |
| `simulators/AltairZ80/s100_pmmi.c` | 5 |
| `simulators/AltairZ80/s100_tuart.c` | 5 |
| `simulators/B5500/b5500_mt.c` | 5 |
| `simulators/BESM6/besm6_mmu.c` | 5 |
| `simulators/H316/h316_hi.c` | 5 |
| `simulators/H316/h316_mi.c` | 5 |
| `simulators/HP2100/hp2100_mux.c` | 5 |
| `simulators/I7000/i7000_cdr.c` | 5 |
| `simulators/I7000/i7090_sys.c` | 5 |
| `simulators/I7094/i7094_dsk.c` | 5 |
| `simulators/Intel-Systems/common/i8251.c` | 5 |
| `simulators/Intel-Systems/common/i8253.c` | 5 |
| `simulators/Intel-Systems/common/i8255.c` | 5 |
| `simulators/Intel-Systems/common/i8259.c` | 5 |
| `simulators/Intel-Systems/common/ieprom.c` | 5 |
| `simulators/Interdata/id_pt.c` | 5 |
| `simulators/ND100/nd100_stddev.c` | 5 |
| `simulators/NOVA/nova_dkp.c` | 5 |
| `simulators/NOVA/nova_dsk.c` | 5 |
| `simulators/NOVA/nova_pt.c` | 5 |
| `simulators/NOVA/nova_tt1.c` | 5 |
| `simulators/PDP1/pdp1_dcs.c` | 5 |
| `simulators/PDP10/ka10_ai.c` | 5 |
| `simulators/PDP18B/pdp18b_dt.c` | 5 |
| `simulators/PDP18B/pdp18b_rf.c` | 5 |
| `simulators/PDP18B/pdp18b_rp.c` | 5 |
| `simulators/SDS/sds_dsk.c` | 5 |
| `simulators/SDS/sds_rad.c` | 5 |
| `simulators/SDS/sds_sys.c` | 5 |
| `simulators/SEL32/sel32_com.c` | 5 |
| `simulators/SEL32/sel32_sys.c` | 5 |
| `simulators/SSEM/ssem_sys.c` | 5 |
| `simulators/TX-0/tx0_sys.c` | 5 |
| `simulators/VAX/vax4xx_rz94.c` | 5 |
| `simulators/VAX/vax_mmu.c` | 5 |
| `simulators/alpha/alpha_io.c` | 5 |
| `simulators/linc/linc_sys.c` | 5 |
| `simulators/sigma/sigma_coc.c` | 5 |
| `simulators/sigma/sigma_cp.c` | 5 |
| `simulators/sigma/sigma_cr.c` | 5 |
| `simulators/AltairZ80/ibc_mcc_hdc.c` | 4 |
| `simulators/AltairZ80/ibc_smd_hdc.c` | 4 |
| `simulators/H316/h316_fhd.c` | 4 |
| `simulators/H316/h316_sys.c` | 4 |
| `simulators/HP2100/hp2100_di.c` | 4 |
| `simulators/HP2100/hp2100_disclib.c` | 4 |
| `simulators/HP2100/hp2100_dq.c` | 4 |
| `simulators/HP2100/hp2100_sys.c` | 4 |
| `simulators/HP3000/hp3000_ds.c` | 4 |
| `simulators/HP3000/hp3000_ms.c` | 4 |
| `simulators/I7000/i7090_cdp.c` | 4 |
| `simulators/I7094/i7094_sys.c` | 4 |
| `simulators/Ibm1130/ibm1130_gdu.c` | 4 |
| `simulators/Ibm1130/ibm1130_ptrp.c` | 4 |
| `simulators/Interdata/id_pas.c` | 4 |
| `simulators/PDP10/kx10_ddc.c` | 4 |
| `simulators/PDP10/pdp10_tim.c` | 4 |
| `simulators/PDP10/pdp10_tu.c` | 4 |
| `simulators/PDP10/pdp6_dct.c` | 4 |
| `simulators/PDP11/pdp11_dc.c` | 4 |
| `simulators/PDP11/pdp11_mb.c` | 4 |
| `simulators/PDP18B/pdp18b_tt1.c` | 4 |
| `simulators/PDP8/pdp8_ct.c` | 4 |
| `simulators/PDP8/pdp8_mt.c` | 4 |
| `simulators/PDQ-3/pdq3_stddev.c` | 4 |
| `simulators/S3/s3_pkb.c` | 4 |
| `simulators/SAGE/m68k_scp.c` | 4 |
| `simulators/SDS/sds_cr.c` | 4 |
| `simulators/SEL32/sel32_chan.c` | 4 |
| `simulators/VAX/vax4xx_rz80.c` | 4 |
| `simulators/VAX/vax780_mem.c` | 4 |
| `simulators/VAX/vax_uw.c` | 4 |
| `simulators/VAX/vax_xs.c` | 4 |
| `simulators/alpha/alpha_ev5_pal.c` | 4 |
| `simulators/imlac/imlac_dp.c` | 4 |
| `simulators/linc/linc_dpy.c` | 4 |
| `simulators/sigma/sigma_tt.c` | 4 |
| `simulators/swtp6800/common/bootrom.c` | 4 |
| `simulators/swtp6800/common/fd400.c` | 4 |
| `simulators/tt2500/tt2500_key.c` | 4 |
| `simulators/tt2500/tt2500_uart.c` | 4 |
| `src/runtime/sim_serial.c` | 4 |
| `third_party/slirp_glue/glib_qemu_stubs.c` | 4 |
| `simulators/3B2/3b2_mem.c` | 3 |
| `simulators/3B2/3b2_sys.c` | 3 |
| `simulators/ALTAIR/altair_sys.c` | 3 |
| `simulators/AltairZ80/i86_prim_ops.c` | 3 |
| `simulators/AltairZ80/n8vem.c` | 3 |
| `simulators/AltairZ80/s100_disk1a.c` | 3 |
| `simulators/AltairZ80/s100_disk3.c` | 3 |
| `simulators/AltairZ80/s100_if3.c` | 3 |
| `simulators/AltairZ80/s100_mdsa.c` | 3 |
| `simulators/AltairZ80/s100_mdsad.c` | 3 |
| `simulators/BESM6/besm6_sys.c` | 3 |
| `simulators/HP2100/hp2100_di_da.c` | 3 |
| `simulators/HP2100/hp2100_ds.c` | 3 |
| `simulators/HP2100/hp2100_mt.c` | 3 |
| `simulators/HP3000/hp3000_mem.c` | 3 |
| `simulators/HP3000/hp3000_mpx.c` | 3 |
| `simulators/HP3000/hp3000_sel.c` | 3 |
| `simulators/I1401/i1401_sys.c` | 3 |
| `simulators/I7000/i7090_cdr.c` | 3 |
| `simulators/I7094/i7094_drm.c` | 3 |
| `simulators/Ibm1130/ibm1130_plot.c` | 3 |
| `simulators/Intel-Systems/common/irq.c` | 3 |
| `simulators/Intel-Systems/common/multibus.c` | 3 |
| `simulators/Interdata/id_dp.c` | 3 |
| `simulators/Interdata/id_fd.c` | 3 |
| `simulators/Interdata/id_idc.c` | 3 |
| `simulators/Interdata/id_lp.c` | 3 |
| `simulators/ND100/nd100_floppy.c` | 3 |
| `simulators/NOVA/nova_mta.c` | 3 |
| `simulators/PDP1/pdp1_clk.c` | 3 |
| `simulators/PDP10/kx10_rp.c` | 3 |
| `simulators/PDP11/pdp11_dh.c` | 3 |
| `simulators/PDP18B/pdp18b_g2tty.c` | 3 |
| `simulators/PDP8/pdp8_dt.c` | 3 |
| `simulators/PDP8/pdp8_rk.c` | 3 |
| `simulators/S3/s3_lp.c` | 3 |
| `simulators/SAGE/m68k_cpu.c` | 3 |
| `simulators/SAGE/m68k_mem.c` | 3 |
| `simulators/SEL32/sel32_con.c` | 3 |
| `simulators/SEL32/sel32_mt.c` | 3 |
| `simulators/TX-0/tx0_sys_orig.c` | 3 |
| `simulators/VAX/vax610_io.c` | 3 |
| `simulators/VAX/vax_fpa.c` | 3 |
| `simulators/imlac/imlac_pt.c` | 3 |
| `simulators/sigma/sigma_sys.c` | 3 |
| `src/runtime/sim_fio.c` | 3 |
| `third_party/slirp/misc.c` | 3 |
| `simulators/3B2/3b2_dmac.c` | 2 |
| `simulators/3B2/3b2_mau.c` | 2 |
| `simulators/ALTAIR/altair_dsk.c` | 2 |
| `simulators/AltairZ80/s100_hayes.c` | 2 |
| `simulators/AltairZ80/s100_selchan.c` | 2 |
| `simulators/B5500/b5500_io.c` | 2 |
| `simulators/H316/h316_lp.c` | 2 |
| `simulators/HP2100/hp2100_baci.c` | 2 |
| `simulators/HP2100/hp2100_pt.c` | 2 |
| `simulators/HP3000/hp3000_scmb.c` | 2 |
| `simulators/I1401/i1401_iq.c` | 2 |
| `simulators/I1401/i1401_mt.c` | 2 |
| `simulators/I1620/i1620_dp.c` | 2 |
| `simulators/I1620/i1620_lp.c` | 2 |
| `simulators/I7094/i7094_lp.c` | 2 |
| `simulators/Interdata/id_mt.c` | 2 |
| `simulators/NOVA/nova_plt.c` | 2 |
| `simulators/PDP1/pdp1_drm.c` | 2 |
| `simulators/PDP1/pdp1_dt.c` | 2 |
| `simulators/PDP10/ka10_dpk.c` | 2 |
| `simulators/PDP10/ka10_mty.c` | 2 |
| `simulators/PDP10/ka10_tk10.c` | 2 |
| `simulators/PDP10/pdp6_ge.c` | 2 |
| `simulators/PDP11/pdp11_rx.c` | 2 |
| `simulators/PDP18B/pdp18b_mt.c` | 2 |
| `simulators/PDP18B/pdp18b_rb.c` | 2 |
| `simulators/PDP8/pdp8_fpp.c` | 2 |
| `simulators/PDP8/pdp8_lp.c` | 2 |
| `simulators/PDP8/pdp8_tsc.c` | 2 |
| `simulators/S3/s3_sys.c` | 2 |
| `simulators/SDS/sds_cp.c` | 2 |
| `simulators/SDS/sds_mt.c` | 2 |
| `simulators/VAX/vax_lk.c` | 2 |
| `simulators/VAX/vax_vs.c` | 2 |
| `simulators/imlac/imlac_crt.c` | 2 |
| `simulators/linc/linc_crt.c` | 2 |
| `simulators/linc/linc_tape.c` | 2 |
| `simulators/sigma/sigma_dk.c` | 2 |
| `simulators/tt2500/tt2500_crt.c` | 2 |
| `simulators/tt2500/tt2500_dpy.c` | 2 |
| `third_party/slirp/tftp.c` | 2 |
| `simulators/3B2/3b2_ctc.c` | 1 |
| `simulators/AltairZ80/flashwriter2.c` | 1 |
| `simulators/AltairZ80/i8272.c` | 1 |
| `simulators/AltairZ80/i86_ops.c` | 1 |
| `simulators/AltairZ80/mfdc.c` | 1 |
| `simulators/AltairZ80/s100_disk2.c` | 1 |
| `simulators/AltairZ80/s100_mdriveh.c` | 1 |
| `simulators/AltairZ80/s100_ss1.c` | 1 |
| `simulators/AltairZ80/vfdhd.c` | 1 |
| `simulators/BESM6/besm6_drum.c` | 1 |
| `simulators/BESM6/besm6_pl.c` | 1 |
| `simulators/BESM6/besm6_printer.c` | 1 |
| `simulators/BESM6/besm6_punch.c` | 1 |
| `simulators/BESM6/besm6_punchcard.c` | 1 |
| `simulators/CDC1700/cdc1700_sym.c` | 1 |
| `simulators/H316/h316_mt.c` | 1 |
| `simulators/HP2100/hp2100_tbg.c` | 1 |
| `simulators/I1401/i1401_dp.c` | 1 |
| `simulators/I1401/i1401_lp.c` | 1 |
| `simulators/I1620/i1620_sys.c` | 1 |
| `simulators/I7094/i7094_binloader.c` | 1 |
| `simulators/I7094/i7094_clk.c` | 1 |
| `simulators/Ibm1130/ibm1130_prt.c` | 1 |
| `simulators/Ibm1130/ibm1130_t2741.c` | 1 |
| `simulators/Intel-Systems/scelbi/scelbi_sys.c` | 1 |
| `simulators/ND100/nd100_mm.c` | 1 |
| `simulators/NOVA/nova_lp.c` | 1 |
| `simulators/NOVA/nova_sys.c` | 1 |
| `simulators/PDP10/kx10_cp.c` | 1 |
| `simulators/PDP10/kx10_cr.c` | 1 |
| `simulators/PDP10/kx10_dk.c` | 1 |
| `simulators/PDP10/kx10_tu.c` | 1 |
| `simulators/PDP10/pdp6_slave.c` | 1 |
| `simulators/PDP11/pdp11_io.c` | 1 |
| `simulators/PDP11/pdp11_sys.c` | 1 |
| `simulators/PDP18B/pdp18b_fpp.c` | 1 |
| `simulators/PDQ-3/pdq3_fdc.c` | 1 |
| `simulators/SAGE/i8251.c` | 1 |
| `simulators/SAGE/i8253.c` | 1 |
| `simulators/SAGE/i8255.c` | 1 |
| `simulators/SAGE/i8259.c` | 1 |
| `simulators/SAGE/i8272.c` | 1 |
| `simulators/SAGE/sage_lp.c` | 1 |
| `simulators/SDS/sds_drm.c` | 1 |
| `simulators/SDS/sds_lp.c` | 1 |
| `simulators/VAX/is1000_syslist.c` | 1 |
| `simulators/VAX/vax410_syslist.c` | 1 |
| `simulators/VAX/vax420_syslist.c` | 1 |
| `simulators/VAX/vax43_syslist.c` | 1 |
| `simulators/VAX/vax440_syslist.c` | 1 |
| `simulators/VAX/vax610_syslist.c` | 1 |
| `simulators/VAX/vax630_syslist.c` | 1 |
| `simulators/VAX/vax730_syslist.c` | 1 |
| `simulators/VAX/vax750_syslist.c` | 1 |
| `simulators/VAX/vax780_fload.c` | 1 |
| `simulators/VAX/vax780_syslist.c` | 1 |
| `simulators/VAX/vax820_syslist.c` | 1 |
| `simulators/VAX/vax860_syslist.c` | 1 |
| `simulators/VAX/vax_cmode.c` | 1 |
| `simulators/VAX/vax_gpx.c` | 1 |
| `simulators/VAX/vax_octa.c` | 1 |
| `simulators/VAX/vax_syslist.c` | 1 |
| `simulators/alpha/alpha_ev5_tlb.c` | 1 |
| `simulators/alpha/alpha_sys.c` | 1 |
| `simulators/linc/linc_kbd.c` | 1 |
| `simulators/sigma/sigma_map.c` | 1 |
| `simulators/sigma/sigma_mt.c` | 1 |
| `simulators/sigma/sigma_pt.c` | 1 |
| `simulators/swtp6800/common/i2716.c` | 1 |
| `simulators/swtp6800/common/m6810.c` | 1 |
| `simulators/swtp6800/common/mp-8m.c` | 1 |
| `simulators/swtp6800/common/mp-b2.c` | 1 |
| `simulators/tt2500/tt2500_tv.c` | 1 |
| `src/runtime/sim_sock.c` | 1 |
| `third_party/slirp/ip_icmp.c` | 1 |
| `third_party/slirp/ip_input.c` | 1 |
| `third_party/slirp/tcp_subr.c` | 1 |

### `-Wmissing-prototypes` (3539 unique sites)

Example sites:

- `simulators/3B2/3b2_dmac.c:205`: no previous prototype for function 'dmac_program'
- `simulators/3B2/3b2_dmac.c:342`: no previous prototype for function 'dmac_page_update'
- `simulators/3B2/3b2_dmac.c:466`: no previous prototype for function 'dmac_service_drqs'
- `simulators/3B2/3b2_id.c:964`: no previous prototype for function 'id_after_dma'
- `simulators/3B2/3b2_if.c:253`: no previous prototype for function 'if_handle_command'

| File | Unique sites |
|---|---:|
| `simulators/AltairZ80/m68k/m68kopnz.c` | 663 |
| `simulators/AltairZ80/m68k/m68kopac.c` | 661 |
| `simulators/AltairZ80/m68k/m68kopdm.c` | 638 |
| `simulators/PDP10/kx10_cpu.c` | 60 |
| `simulators/PDP11/pdp11_dmc.c` | 43 |
| `src/runtime/sim_scsi.c` | 41 |
| `simulators/B5500/b5500_cpu.c` | 37 |
| `simulators/PDP10/pdp10_pag.c` | 31 |
| `simulators/PDP10/pdp10_ksio.c` | 21 |
| `simulators/PDP11/pdp11_xq.c` | 21 |
| `simulators/VAX/vax440_sysdev.c` | 21 |
| `src/core/scp.c` | 21 |
| `simulators/VAX/vax730_stddev.c` | 18 |
| `simulators/VAX/vax750_stddev.c` | 18 |
| `simulators/BESM6/besm6_tty.c` | 17 |
| `simulators/I650/i650_cpu.c` | 17 |
| `simulators/I7094/i7094_cpu1.c` | 17 |
| `simulators/VAX/vax860_stddev.c` | 17 |
| `simulators/CDC1700/cdc1700_cpu.c` | 16 |
| `simulators/CDC1700/cdc1700_iofw.c` | 16 |
| `simulators/PDP11/pdp11_xu.c` | 16 |
| `simulators/VAX/vax4xx_va.c` | 16 |
| `simulators/VAX/vax820_stddev.c` | 16 |
| `simulators/VAX/vax43_sysdev.c` | 15 |
| `simulators/sigma/sigma_io.c` | 15 |
| `simulators/I650/i650_cdp.c` | 14 |
| `simulators/PDP10/kl10_nia.c` | 14 |
| `simulators/PDP10/pdp10_mdfp.c` | 14 |
| `simulators/alpha/alpha_ev5_pal.c` | 14 |
| `simulators/I650/i650_cdr.c` | 13 |
| `simulators/BESM6/besm6_sys.c` | 12 |
| `simulators/CDC1700/cdc1700_io.c` | 12 |
| `simulators/I1620/i1620_fp.c` | 12 |
| `simulators/VAX/vax780_stddev.c` | 12 |
| `simulators/AltairZ80/altairz80_hdsk.c` | 11 |
| `simulators/BESM6/besm6_disk.c` | 11 |
| `simulators/CDC1700/cdc1700_dev1.c` | 11 |
| `simulators/H316/h316_hi.c` | 11 |
| `simulators/H316/h316_mi.c` | 11 |
| `simulators/PDP10/kx10_sys.c` | 11 |
| `simulators/VAX/vax420_sysdev.c` | 11 |
| `src/runtime/sim_ether.c` | 11 |
| `simulators/BESM6/besm6_vu.c` | 10 |
| `simulators/CDC1700/cdc1700_mt.c` | 10 |
| `simulators/GRI/gri_stddev.c` | 10 |
| `simulators/NOVA/eclipse_cpu.c` | 10 |
| `simulators/PDP11/pdp11_cpu.c` | 10 |
| `simulators/SSEM/ssem_sys.c` | 10 |
| `simulators/VAX/vax_io.c` | 10 |
| `simulators/VAX/vax_stddev.c` | 10 |
| `simulators/BESM6/besm6_mmu.c` | 9 |
| `simulators/I650/i650_sys.c` | 9 |
| `simulators/I7094/i7094_io.c` | 9 |
| `simulators/Interdata/id_fp.c` | 9 |
| `simulators/PDP11/pdp11_ch.c` | 9 |
| `simulators/SAGE/m68k_cpu.c` | 9 |
| `simulators/BESM6/besm6_cpu.c` | 8 |
| `simulators/I7000/i7010_cpu.c` | 8 |
| `simulators/I7000/i7080_chan.c` | 8 |
| `simulators/PDP11/pdp11_ts.c` | 8 |
| `simulators/VAX/vax4xx_stddev.c` | 8 |
| `simulators/VAX/vax4xx_ve.c` | 8 |
| `simulators/VAX/vax610_stddev.c` | 8 |
| `simulators/VAX/vax630_stddev.c` | 8 |
| `simulators/3B2/3b2_mau.c` | 7 |
| `simulators/NOVA/nova_qty.c` | 7 |
| `simulators/PDP10/ka10_ch10.c` | 7 |
| `simulators/PDP11/pdp11_io.c` | 7 |
| `simulators/SDS/sds_io.c` | 7 |
| `simulators/SEL32/sel32_chan.c` | 7 |
| `simulators/SEL32/sel32_hsdp.c` | 7 |
| `simulators/SEL32/sel32_sys.c` | 7 |
| `simulators/VAX/vax_gpx.c` | 7 |
| `simulators/VAX/vax_va.c` | 7 |
| `simulators/3B2/3b2_rev2_mmu.c` | 6 |
| `simulators/H316/h316_cpu.c` | 6 |
| `simulators/I650/i650_mt.c` | 6 |
| `simulators/I7000/i7080_cpu.c` | 6 |
| `simulators/LGP/lgp_sys.c` | 6 |
| `simulators/PDP1/pdp1_stddev.c` | 6 |
| `simulators/PDP18B/pdp18b_g2tty.c` | 6 |
| `simulators/PDP18B/pdp18b_sys.c` | 6 |
| `simulators/SDS/sds_sys.c` | 6 |
| `simulators/SEL32/sel32_disk.c` | 6 |
| `simulators/VAX/is1000_sysdev.c` | 6 |
| `simulators/VAX/vax410_sysdev.c` | 6 |
| `simulators/VAX/vax4xx_rz94.c` | 6 |
| `simulators/VAX/vax860_sbia.c` | 6 |
| `simulators/VAX/vax_syscm.c` | 6 |
| `simulators/VAX/vax_uw.c` | 6 |
| `simulators/alpha/alpha_fpv.c` | 6 |
| `simulators/GRI/gri_sys.c` | 5 |
| `simulators/Ibm1130/ibm1130_sys.c` | 5 |
| `simulators/Interdata/id_io.c` | 5 |
| `simulators/PDP1/pdp1_sys.c` | 5 |
| `simulators/PDP10/pdp10_sys.c` | 5 |
| `simulators/PDP10/pdp10_tim.c` | 5 |
| `simulators/PDQ-3/pdq3_debug.c` | 5 |
| `simulators/PDQ-3/pdq3_sys.c` | 5 |
| `simulators/VAX/vax4nn_stddev.c` | 5 |
| `simulators/VAX/vax4xx_rd.c` | 5 |
| `simulators/VAX/vax630_io.c` | 5 |
| `simulators/VAX/vax820_uba.c` | 5 |
| `src/runtime/sim_timer.c` | 5 |
| `third_party/slirp_glue/glib_qemu_stubs.c` | 5 |
| `simulators/ALTAIR/altair_dsk.c` | 4 |
| `simulators/ALTAIR/altair_sio.c` | 4 |
| `simulators/AltairZ80/ibc.c` | 4 |
| `simulators/B5500/b5500_io.c` | 4 |
| `simulators/BESM6/besm6_drum.c` | 4 |
| `simulators/CDC1700/cdc1700_msos5.c` | 4 |
| `simulators/I1401/i1401_sys.c` | 4 |
| `simulators/I1620/i1620_pt.c` | 4 |
| `simulators/I650/i650_dsk.c` | 4 |
| `simulators/I7000/i7010_chan.c` | 4 |
| `simulators/I7000/i7070_chan.c` | 4 |
| `simulators/I7000/i7080_sys.c` | 4 |
| `simulators/Interdata/id32_sys.c` | 4 |
| `simulators/PDP10/kx10_imp.c` | 4 |
| `simulators/PDP10/kx10_mt.c` | 4 |
| `simulators/PDP11/pdp11_sys.c` | 4 |
| `simulators/PDP8/pdp8_sys.c` | 4 |
| `simulators/SDS/sds_cpu.c` | 4 |
| `simulators/TX-0/tx0_stddev.c` | 4 |
| `simulators/VAX/vax730_sys.c` | 4 |
| `simulators/VAX/vax750_cmi.c` | 4 |
| `simulators/VAX/vax780_sbi.c` | 4 |
| `simulators/VAX/vax780_uba.c` | 4 |
| `simulators/VAX/vax820_bi.c` | 4 |
| `simulators/VAX/vax820_ka.c` | 4 |
| `simulators/alpha/alpha_ev5_cons.c` | 4 |
| `simulators/alpha/alpha_ev5_tlb.c` | 4 |
| `simulators/alpha/alpha_fpi.c` | 4 |
| `src/runtime/sim_disk.c` | 4 |
| `simulators/3B2/3b2_dmac.c` | 3 |
| `simulators/3B2/3b2_rev3_mmu.c` | 3 |
| `simulators/CDC1700/cdc1700_cd.c` | 3 |
| `simulators/CDC1700/cdc1700_dc.c` | 3 |
| `simulators/CDC1700/cdc1700_sys.c` | 3 |
| `simulators/H316/h316_udp.c` | 3 |
| `simulators/I1401/i1401_cd.c` | 3 |
| `simulators/I7000/i7010_sys.c` | 3 |
| `simulators/I7000/i701_sys.c` | 3 |
| `simulators/I7000/i7070_cpu.c` | 3 |
| `simulators/I7000/i7090_sys.c` | 3 |
| `simulators/LGP/lgp_stddev.c` | 3 |
| `simulators/NOVA/nova_sys.c` | 3 |
| `simulators/PDP10/ka10_pmp.c` | 3 |
| `simulators/PDP10/kl10_fe.c` | 3 |
| `simulators/PDP11/pdp11_rq.c` | 3 |
| `simulators/PDP11/pdp11_tq.c` | 3 |
| `simulators/PDP18B/pdp18b_stddev.c` | 3 |
| `simulators/PDQ-3/pdq3_cpu.c` | 3 |
| `simulators/PDQ-3/pdq3_fdc.c` | 3 |
| `simulators/PDQ-3/pdq3_stddev.c` | 3 |
| `simulators/SEL32/sel32_cpu.c` | 3 |
| `simulators/VAX/vax4xx_vc.c` | 3 |
| `simulators/VAX/vax610_io.c` | 3 |
| `simulators/VAX/vax610_sysdev.c` | 3 |
| `simulators/VAX/vax630_sysdev.c` | 3 |
| `simulators/VAX/vax730_uba.c` | 3 |
| `simulators/VAX/vax750_uba.c` | 3 |
| `simulators/VAX/vax_sysdev.c` | 3 |
| `simulators/VAX/vax_vc.c` | 3 |
| `simulators/sigma/sigma_map.c` | 3 |
| `simulators/3B2/3b2_if.c` | 2 |
| `simulators/3B2/3b2_iu.c` | 2 |
| `simulators/3B2/3b2_scsi.c` | 2 |
| `simulators/AltairZ80/m68k/m68kcpu.c` | 2 |
| `simulators/AltairZ80/s100_scp300f.c` | 2 |
| `simulators/AltairZ80/wd179x.c` | 2 |
| `simulators/B5500/b5500_dtc.c` | 2 |
| `simulators/BESM6/besm6_mg.c` | 2 |
| `simulators/CDC1700/cdc1700_dp.c` | 2 |
| `simulators/CDC1700/cdc1700_drm.c` | 2 |
| `simulators/I1401/i1401_lp.c` | 2 |
| `simulators/I1401/i1401_mt.c` | 2 |
| `simulators/I1620/i1620_cd.c` | 2 |
| `simulators/I1620/i1620_sys.c` | 2 |
| `simulators/I1620/i1620_tty.c` | 2 |
| `simulators/I7000/i7000_mt.c` | 2 |
| `simulators/I7000/i7070_sys.c` | 2 |
| `simulators/I7000/i7090_chan.c` | 2 |
| `simulators/I7094/i7094_com.c` | 2 |
| `simulators/Ibm1130/ibm1130_cr.c` | 2 |
| `simulators/Ibm1130/ibm1130_fmt.c` | 2 |
| `simulators/Ibm1130/ibm1130_sca.c` | 2 |
| `simulators/Interdata/id16_sys.c` | 2 |
| `simulators/ND100/nd100_cpu.c` | 2 |
| `simulators/PDP1/pdp1_dt.c` | 2 |
| `simulators/PDP10/kx10_dt.c` | 2 |
| `simulators/PDP10/kx10_lp.c` | 2 |
| `simulators/PDP10/kx10_tu.c` | 2 |
| `simulators/PDP10/pdp10_xtnd.c` | 2 |
| `simulators/PDP11/pdp11_fp.c` | 2 |
| `simulators/PDP11/pdp11_rh.c` | 2 |
| `simulators/PDQ-3/pdq3_mem.c` | 2 |
| `simulators/S3/s3_cd.c` | 2 |
| `simulators/S3/s3_disk.c` | 2 |
| `simulators/S3/s3_pkb.c` | 2 |
| `simulators/SAGE/i8253.c` | 2 |
| `simulators/SAGE/sage_fd.c` | 2 |
| `simulators/SDS/sds_cp.c` | 2 |
| `simulators/SDS/sds_dsk.c` | 2 |
| `simulators/SDS/sds_mux.c` | 2 |
| `simulators/SDS/sds_rad.c` | 2 |
| `simulators/SEL32/sel32_ipu.c` | 2 |
| `simulators/SEL32/sel32_scfi.c` | 2 |
| `simulators/SEL32/sel32_scsi.c` | 2 |
| `simulators/TX-0/tx0_cpu.c` | 2 |
| `simulators/TX-0/tx0_sys.c` | 2 |
| `simulators/VAX/vax4xx_dz.c` | 2 |
| `simulators/VAX/vax4xx_rz80.c` | 2 |
| `simulators/VAX/vax630_syslist.c` | 2 |
| `simulators/VAX/vax7x0_mba.c` | 2 |
| `simulators/VAX/vax860_abus.c` | 2 |
| `simulators/VAX/vax_xs.c` | 2 |
| `simulators/linc/linc_dpy.c` | 2 |
| `simulators/sigma/sigma_cis.c` | 2 |
| `simulators/sigma/sigma_fp.c` | 2 |
| `src/runtime/sim_card.c` | 2 |
| `src/runtime/sim_tape.c` | 2 |
| `src/runtime/sim_tmxr.c` | 2 |
| `simulators/3B2/3b2_id.c` | 1 |
| `simulators/3B2/3b2_ni.c` | 1 |
| `simulators/3B2/3b2_rev2_sys.c` | 1 |
| `simulators/3B2/3b2_rev3_sys.c` | 1 |
| `simulators/3B2/3b2_timer.c` | 1 |
| `simulators/AltairZ80/altairz80_cpu.c` | 1 |
| `simulators/AltairZ80/altairz80_sys.c` | 1 |
| `simulators/AltairZ80/i86_decode.c` | 1 |
| `simulators/AltairZ80/m68k/m68kdasm.c` | 1 |
| `simulators/B5500/b5500_dk.c` | 1 |
| `simulators/B5500/b5500_mt.c` | 1 |
| `simulators/B5500/b5500_sys.c` | 1 |
| `simulators/B5500/b5500_urec.c` | 1 |
| `simulators/BESM6/besm6_printer.c` | 1 |
| `simulators/CDC1700/cdc1700_dis.c` | 1 |
| `simulators/H316/h316_sys.c` | 1 |
| `simulators/I1401/i1401_dp.c` | 1 |
| `simulators/I1401/i1401_iq.c` | 1 |
| `simulators/I1620/i1620_dp.c` | 1 |
| `simulators/I1620/i1620_lp.c` | 1 |
| `simulators/I7000/i7000_chron.c` | 1 |
| `simulators/I7000/i7000_com.c` | 1 |
| `simulators/I7000/i7000_dsk.c` | 1 |
| `simulators/I7000/i7000_lpr.c` | 1 |
| `simulators/I7000/i701_chan.c` | 1 |
| `simulators/I7000/i7090_lpr.c` | 1 |
| `simulators/I7094/i7094_binloader.c` | 1 |
| `simulators/I7094/i7094_clk.c` | 1 |
| `simulators/I7094/i7094_drm.c` | 1 |
| `simulators/I7094/i7094_sys.c` | 1 |
| `simulators/Ibm1130/ibm1130_cpu.c` | 1 |
| `simulators/Ibm1130/ibm1130_disk.c` | 1 |
| `simulators/Ibm1130/ibm1130_gui.c` | 1 |
| `simulators/Ibm1130/ibm1130_prt.c` | 1 |
| `simulators/Ibm1130/ibm1130_stddev.c` | 1 |
| `simulators/Intel-Systems/common/i8255.c` | 1 |
| `simulators/Intel-Systems/common/port.c` | 1 |
| `simulators/Interdata/id16_dboot.c` | 1 |
| `simulators/Interdata/id32_cpu.c` | 1 |
| `simulators/Interdata/id32_dboot.c` | 1 |
| `simulators/Interdata/id_dp.c` | 1 |
| `simulators/Interdata/id_idc.c` | 1 |
| `simulators/Interdata/id_lp.c` | 1 |
| `simulators/Interdata/id_pt.c` | 1 |
| `simulators/ND100/nd100_mm.c` | 1 |
| `simulators/NOVA/eclipse_tt.c` | 1 |
| `simulators/NOVA/nova_cpu.c` | 1 |
| `simulators/PDP1/pdp1_clk.c` | 1 |
| `simulators/PDP1/pdp1_dcs.c` | 1 |
| `simulators/PDP1/pdp1_drm.c` | 1 |
| `simulators/PDP1/pdp1_lp.c` | 1 |
| `simulators/PDP10/ks10_cty.c` | 1 |
| `simulators/PDP10/ks10_uba.c` | 1 |
| `simulators/PDP10/kx10_pt.c` | 1 |
| `simulators/PDP10/kx10_rh.c` | 1 |
| `simulators/PDP10/pdp10_cpu.c` | 1 |
| `simulators/PDP10/pdp10_fe.c` | 1 |
| `simulators/PDP10/pdp6_mtc.c` | 1 |
| `simulators/PDP11/pdp11_cis.c` | 1 |
| `simulators/PDP11/pdp11_cpumod.c` | 1 |
| `simulators/PDP11/pdp11_dl.c` | 1 |
| `simulators/PDP11/pdp11_hk.c` | 1 |
| `simulators/PDP11/pdp11_rom.c` | 1 |
| `simulators/PDP11/pdp11_rp.c` | 1 |
| `simulators/PDP18B/pdp18b_cpu.c` | 1 |
| `simulators/PDP18B/pdp18b_fpp.c` | 1 |
| `simulators/PDP8/pdp8_cpu.c` | 1 |
| `simulators/S3/s3_lp.c` | 1 |
| `simulators/SAGE/m68k_mem.c` | 1 |
| `simulators/SAGE/m68k_scp.c` | 1 |
| `simulators/SAGE/m68k_sys.c` | 1 |
| `simulators/SDS/sds_cr.c` | 1 |
| `simulators/SEL32/sel32_clk.c` | 1 |
| `simulators/SEL32/sel32_com.c` | 1 |
| `simulators/SEL32/sel32_mt.c` | 1 |
| `simulators/TX-0/tx0_sys_orig.c` | 1 |
| `simulators/VAX/is1000_syslist.c` | 1 |
| `simulators/VAX/vax410_syslist.c` | 1 |
| `simulators/VAX/vax420_syslist.c` | 1 |
| `simulators/VAX/vax43_syslist.c` | 1 |
| `simulators/VAX/vax440_syslist.c` | 1 |
| `simulators/VAX/vax610_syslist.c` | 1 |
| `simulators/VAX/vax730_syslist.c` | 1 |
| `simulators/VAX/vax750_mem.c` | 1 |
| `simulators/VAX/vax750_syslist.c` | 1 |
| `simulators/VAX/vax780_fload.c` | 1 |
| `simulators/VAX/vax780_syslist.c` | 1 |
| `simulators/VAX/vax820_syslist.c` | 1 |
| `simulators/VAX/vax860_syslist.c` | 1 |
| `simulators/VAX/vax_cpu.c` | 1 |
| `simulators/VAX/vax_lk.c` | 1 |
| `simulators/VAX/vax_nar.c` | 1 |
| `simulators/VAX/vax_syslist.c` | 1 |
| `simulators/alpha/alpha_cpu.c` | 1 |
| `simulators/linc/linc_cpu.c` | 1 |
| `simulators/sigma/sigma_dp.c` | 1 |
| `src/runtime/sim_serial.c` | 1 |

### `-Wimplicit-fallthrough` (546 unique sites)

Example sites:

- `simulators/3B2/3b2_id.c:271`: unannotated fall-through between switch labels
- `simulators/ALTAIR/altair_cpu.c:1033`: unannotated fall-through between switch labels
- `simulators/AltairZ80/altairz80_cpu.c:4614`: unannotated fall-through between switch labels
- `simulators/AltairZ80/altairz80_cpu.c:4625`: unannotated fall-through between switch labels
- `simulators/AltairZ80/altairz80_cpu.c:6152`: unannotated fall-through between switch labels

| File | Unique sites |
|---|---:|
| `simulators/VAX/vax_cpu.c` | 25 |
| `simulators/PDP10/kx10_cpu.c` | 22 |
| `simulators/I7000/i7010_cpu.c` | 12 |
| `simulators/SEL32/sel32_cpu.c` | 12 |
| `simulators/SEL32/sel32_ipu.c` | 12 |
| `simulators/PDP18B/pdp18b_cpu.c` | 11 |
| `simulators/I7000/i7070_cpu.c` | 10 |
| `simulators/I7000/i7090_cpu.c` | 10 |
| `simulators/AltairZ80/s100_ss1.c` | 8 |
| `simulators/HP3000/hp_tapelib.c` | 8 |
| `simulators/I7000/i7090_chan.c` | 8 |
| `simulators/PDP10/ks10_kmc.c` | 8 |
| `simulators/PDP10/pdp10_ksio.c` | 8 |
| `simulators/PDP11/pdp11_kmc.c` | 8 |
| `simulators/B5500/b5500_dtc.c` | 7 |
| `simulators/I7000/i7000_dsk.c` | 7 |
| `simulators/Interdata/id16_cpu.c` | 7 |
| `simulators/Interdata/id32_cpu.c` | 7 |
| `simulators/PDP8/pdp8_cpu.c` | 7 |
| `simulators/sigma/sigma_cis.c` | 7 |
| `simulators/CDC1700/cdc1700_cpu.c` | 6 |
| `simulators/HP3000/hp3000_cpu_base.c` | 6 |
| `simulators/HP3000/hp3000_sys.c` | 6 |
| `simulators/I7000/i7080_cpu.c` | 6 |
| `simulators/SEL32/sel32_sys.c` | 6 |
| `simulators/AltairZ80/s100_scp300f.c` | 5 |
| `simulators/B5500/b5500_cpu.c` | 5 |
| `simulators/HP3000/hp3000_cpu.c` | 5 |
| `simulators/PDP10/pdp10_rp.c` | 5 |
| `simulators/AltairZ80/altairz80_cpu.c` | 4 |
| `simulators/AltairZ80/altairz80_cpu_nommu.c` | 4 |
| `simulators/I7000/i7000_ht.c` | 4 |
| `simulators/I7000/i7000_mt.c` | 4 |
| `simulators/I7000/i7070_sys.c` | 4 |
| `simulators/I7000/i7080_chan.c` | 4 |
| `simulators/PDP10/kx10_dp.c` | 4 |
| `simulators/PDP10/kx10_rp.c` | 4 |
| `simulators/PDP11/pdp11_cis.c` | 4 |
| `simulators/PDP11/pdp11_hk.c` | 4 |
| `simulators/PDP11/pdp11_rp.c` | 4 |
| `simulators/PDP18B/pdp18b_dt.c` | 4 |
| `simulators/PDP8/pdp8_dt.c` | 4 |
| `simulators/SAGE/i8272.c` | 4 |
| `simulators/SAGE/m68k_sys.c` | 4 |
| `simulators/SEL32/sel32_disk.c` | 4 |
| `simulators/SEL32/sel32_lpr.c` | 4 |
| `simulators/VAX/vax_sys.c` | 4 |
| `simulators/AltairZ80/m68k/m68kdasm.c` | 3 |
| `simulators/GRI/gri_sys.c` | 3 |
| `simulators/H316/h316_dp.c` | 3 |
| `simulators/HP2100/hp2100_sys.c` | 3 |
| `simulators/HP3000/hp_disclib.c` | 3 |
| `simulators/I7000/i701_cpu.c` | 3 |
| `simulators/I7094/i7094_binloader.c` | 3 |
| `simulators/I7094/i7094_io.c` | 3 |
| `simulators/Ibm1130/ibm1130_cpu.c` | 3 |
| `simulators/NOVA/nova_sys.c` | 3 |
| `simulators/PDP10/kx10_tu.c` | 3 |
| `simulators/PDP10/pdp10_tu.c` | 3 |
| `simulators/PDP11/pdp11_cpu.c` | 3 |
| `simulators/PDP11/pdp11_sys.c` | 3 |
| `simulators/PDP11/pdp11_tu.c` | 3 |
| `simulators/SAGE/m68k_mem.c` | 3 |
| `simulators/SEL32/sel32_hsdp.c` | 3 |
| `simulators/SEL32/sel32_mt.c` | 3 |
| `simulators/SEL32/sel32_scfi.c` | 3 |
| `simulators/tt2500/tt2500_cpu.c` | 3 |
| `src/runtime/sim_imd.c` | 3 |
| `src/runtime/sim_tmxr.c` | 3 |
| `simulators/AltairZ80/i8272.c` | 2 |
| `simulators/AltairZ80/s100_hdc1001.c` | 2 |
| `simulators/CDC1700/cdc1700_dis.c` | 2 |
| `simulators/CDC1700/cdc1700_mt.c` | 2 |
| `simulators/HP2100/hp2100_cpu.c` | 2 |
| `simulators/HP2100/hp2100_cpu1.c` | 2 |
| `simulators/HP2100/hp2100_di_da.c` | 2 |
| `simulators/HP2100/hp2100_dp.c` | 2 |
| `simulators/HP2100/hp2100_dq.c` | 2 |
| `simulators/HP2100/hp2100_ms.c` | 2 |
| `simulators/I1401/i1401_dp.c` | 2 |
| `simulators/I7000/i7000_cdp.c` | 2 |
| `simulators/I7000/i7070_chan.c` | 2 |
| `simulators/I7000/i7090_lpr.c` | 2 |
| `simulators/I7094/i7094_dsk.c` | 2 |
| `simulators/Interdata/id32_sys.c` | 2 |
| `simulators/NOVA/nova_mta.c` | 2 |
| `simulators/PDP1/pdp1_sys.c` | 2 |
| `simulators/PDP10/kx10_dt.c` | 2 |
| `simulators/PDP10/kx10_mt.c` | 2 |
| `simulators/PDP10/kx10_sys.c` | 2 |
| `simulators/PDP10/pdp10_sys.c` | 2 |
| `simulators/PDP11/pdp11_dmc.c` | 2 |
| `simulators/PDP11/pdp11_rr.c` | 2 |
| `simulators/PDP11/pdp11_rs.c` | 2 |
| `simulators/PDP11/pdp11_ts.c` | 2 |
| `simulators/PDP11/pdp11_vh.c` | 2 |
| `simulators/PDP18B/pdp18b_lp.c` | 2 |
| `simulators/PDP8/pdp8_pt.c` | 2 |
| `simulators/PDP8/pdp8_rf.c` | 2 |
| `simulators/SDS/sds_cpu.c` | 2 |
| `simulators/SDS/sds_drm.c` | 2 |
| `simulators/SDS/sds_sys.c` | 2 |
| `simulators/SEL32/sel32_con.c` | 2 |
| `simulators/SEL32/sel32_scsi.c` | 2 |
| `simulators/VAX/vax_cis.c` | 2 |
| `simulators/VAX/vax_cpu1.c` | 2 |
| `simulators/VAX/vax_syscm.c` | 2 |
| `simulators/sigma/sigma_coc.c` | 2 |
| `simulators/sigma/sigma_dp.c` | 2 |
| `simulators/sigma/sigma_map.c` | 2 |
| `src/core/scp.c` | 2 |
| `src/runtime/sim_tape.c` | 2 |
| `simulators/3B2/3b2_id.c` | 1 |
| `simulators/ALTAIR/altair_cpu.c` | 1 |
| `simulators/AltairZ80/altairz80_sys.c` | 1 |
| `simulators/AltairZ80/ibc_smd_hdc.c` | 1 |
| `simulators/AltairZ80/s100_icom.c` | 1 |
| `simulators/B5500/b5500_urec.c` | 1 |
| `simulators/CDC1700/cdc1700_cd.c` | 1 |
| `simulators/CDC1700/cdc1700_dc.c` | 1 |
| `simulators/CDC1700/cdc1700_dp.c` | 1 |
| `simulators/CDC1700/cdc1700_sym.c` | 1 |
| `simulators/H316/h316_mt.c` | 1 |
| `simulators/HP2100/hp2100_dr.c` | 1 |
| `simulators/HP3000/hp3000_ds.c` | 1 |
| `simulators/HP3000/hp3000_lp.c` | 1 |
| `simulators/HP3000/hp3000_mem.c` | 1 |
| `simulators/HP3000/hp3000_ms.c` | 1 |
| `simulators/HP3000/hp3000_sel.c` | 1 |
| `simulators/I1620/i1620_cpu.c` | 1 |
| `simulators/I7000/i7000_cdr.c` | 1 |
| `simulators/I7000/i7000_chron.c` | 1 |
| `simulators/I7000/i7010_sys.c` | 1 |
| `simulators/I7094/i7094_cd.c` | 1 |
| `simulators/I7094/i7094_drm.c` | 1 |
| `simulators/I7094/i7094_sys.c` | 1 |
| `simulators/Intel-Systems/common/i8080.c` | 1 |
| `simulators/Interdata/id16_sys.c` | 1 |
| `simulators/Interdata/id_mt.c` | 1 |
| `simulators/LGP/lgp_sys.c` | 1 |
| `simulators/NOVA/eclipse_cpu.c` | 1 |
| `simulators/NOVA/nova_cpu.c` | 1 |
| `simulators/NOVA/nova_dkp.c` | 1 |
| `simulators/PDP1/pdp1_dcs.c` | 1 |
| `simulators/PDP10/ka10_pmp.c` | 1 |
| `simulators/PDP10/kl10_fe.c` | 1 |
| `simulators/PDP10/kx10_rh.c` | 1 |
| `simulators/PDP10/pdp10_cpu.c` | 1 |
| `simulators/PDP10/pdp6_mtc.c` | 1 |
| `simulators/PDP11/pdp11_cr.c` | 1 |
| `simulators/PDP11/pdp11_ddcmp.h` | 1 |
| `simulators/PDP11/pdp11_ta.c` | 1 |
| `simulators/PDP11/pdp11_tm.c` | 1 |
| `simulators/PDP11/pdp11_tq.c` | 1 |
| `simulators/PDP18B/pdp18b_mt.c` | 1 |
| `simulators/PDP18B/pdp18b_sys.c` | 1 |
| `simulators/PDP8/pdp8_ct.c` | 1 |
| `simulators/PDP8/pdp8_lp.c` | 1 |
| `simulators/PDP8/pdp8_mt.c` | 1 |
| `simulators/PDP8/pdp8_rk.c` | 1 |
| `simulators/PDP8/pdp8_rl.c` | 1 |
| `simulators/PDP8/pdp8_rx.c` | 1 |
| `simulators/PDP8/pdp8_tt.c` | 1 |
| `simulators/PDP8/pdp8_ttx.c` | 1 |
| `simulators/PDQ-3/pdq3_fdc.c` | 1 |
| `simulators/SAGE/i8253.c` | 1 |
| `simulators/SAGE/i8259.c` | 1 |
| `simulators/SDS/sds_mt.c` | 1 |
| `simulators/SDS/sds_mux.c` | 1 |
| `simulators/SEL32/sel32_ec.c` | 1 |
| `simulators/VAX/vax780_stddev.c` | 1 |
| `simulators/VAX/vax860_stddev.c` | 1 |
| `simulators/VAX/vax_uw.c` | 1 |
| `simulators/imlac/imlac_sys.c` | 1 |
| `simulators/sigma/sigma_cp.c` | 1 |
| `simulators/sigma/sigma_fp.c` | 1 |
| `simulators/sigma/sigma_mt.c` | 1 |
| `simulators/sigma/sigma_tt.c` | 1 |
| `src/runtime/sim_card.c` | 1 |
| `src/runtime/sim_console.c` | 1 |

### `-Wstrict-prototypes` (347 unique sites)

Example sites:

- `simulators/3B2/3b2_cpu.c:96`: a function declaration without a prototype is deprecated in all versions of C
- `simulators/3B2/3b2_cpu.c:97`: a function declaration without a prototype is deprecated in all versions of C
- `simulators/3B2/3b2_cpu.c:98`: a function declaration without a prototype is deprecated in all versions of C
- `simulators/3B2/3b2_cpu.c:99`: a function declaration without a prototype is deprecated in all versions of C
- `simulators/3B2/3b2_cpu.c:105`: a function declaration without a prototype is deprecated in all versions of C

| File | Unique sites |
|---|---:|
| `simulators/3B2/3b2_mau.c` | 51 |
| `simulators/PDQ-3/pdq3_cpu.c` | 22 |
| `simulators/B5500/b5500_cpu.c` | 21 |
| `simulators/PDP18B/pdp18b_g2tty.c` | 18 |
| `simulators/3B2/3b2_cpu.c` | 16 |
| `simulators/PDP10/kx10_cpu.c` | 14 |
| `simulators/PDP10/kl10_nia.c` | 13 |
| `simulators/BESM6/besm6_tty.c` | 8 |
| `simulators/PDQ-3/pdq3_defs.h` | 8 |
| `simulators/PDQ-3/pdq3_fdc.c` | 8 |
| `simulators/PDP10/kx10_defs.h` | 7 |
| `simulators/PDQ-3/pdq3_debug.c` | 7 |
| `simulators/3B2/3b2_iu.h` | 6 |
| `simulators/linc/linc_cpu.c` | 6 |
| `simulators/3B2/3b2_ni.c` | 5 |
| `simulators/BESM6/besm6_cpu.c` | 5 |
| `simulators/BESM6/besm6_mmu.c` | 5 |
| `simulators/3B2/3b2_if.c` | 4 |
| `simulators/I7000/i7010_cpu.c` | 4 |
| `simulators/PDP10/ka10_pmp.c` | 4 |
| `simulators/VAX/vax820_uba.c` | 4 |
| `simulators/3B2/3b2_id.c` | 3 |
| `simulators/3B2/3b2_rev2_mmu.c` | 3 |
| `simulators/3B2/3b2_rev3_mmu.c` | 3 |
| `simulators/SAGE/m68k_cpu.c` | 3 |
| `simulators/SAGE/m68k_cpu.h` | 3 |
| `simulators/SEL32/sel32_cpu.c` | 3 |
| `simulators/SEL32/sel32_ipu.c` | 3 |
| `simulators/VAX/vax_gpx.c` | 3 |
| `simulators/3B2/3b2_dmac.h` | 2 |
| `simulators/3B2/3b2_if.h` | 2 |
| `simulators/3B2/3b2_iu.c` | 2 |
| `simulators/3B2/3b2_rev2_mmu.h` | 2 |
| `simulators/3B2/3b2_rev3_mmu.h` | 2 |
| `simulators/3B2/3b2_scsi.c` | 2 |
| `simulators/ALTAIR/altair_dsk.c` | 2 |
| `simulators/B5500/b5500_defs.h` | 2 |
| `simulators/B5500/b5500_io.c` | 2 |
| `simulators/I7000/i7000_com.c` | 2 |
| `simulators/I7000/i7070_cpu.c` | 2 |
| `simulators/I7000/i7080_chan.c` | 2 |
| `simulators/Intel-Systems/common/i8255.c` | 2 |
| `simulators/Intel-Systems/common/port.c` | 2 |
| `simulators/NOVA/eclipse_tt.c` | 2 |
| `simulators/PDP10/kl10_fe.c` | 2 |
| `simulators/PDP10/pdp10_cpu.c` | 2 |
| `simulators/SEL32/sel32_chan.c` | 2 |
| `simulators/VAX/vax420_sysdev.c` | 2 |
| `simulators/VAX/vax43_sysdev.c` | 2 |
| `simulators/VAX/vax780_uba.c` | 2 |
| `src/runtime/sim_ether.c` | 2 |
| `simulators/3B2/3b2_dmac.c` | 1 |
| `simulators/3B2/3b2_id.h` | 1 |
| `simulators/3B2/3b2_ni.h` | 1 |
| `simulators/3B2/3b2_rev2_sys.c` | 1 |
| `simulators/3B2/3b2_rev3_sys.c` | 1 |
| `simulators/3B2/3b2_scsi.h` | 1 |
| `simulators/3B2/3b2_stddev.h` | 1 |
| `simulators/3B2/3b2_sys.h` | 1 |
| `simulators/BESM6/besm6_disk.c` | 1 |
| `simulators/BESM6/besm6_drum.c` | 1 |
| `simulators/BESM6/besm6_mg.c` | 1 |
| `simulators/I650/i650_cdp.c` | 1 |
| `simulators/I7000/i7000_defs.h` | 1 |
| `simulators/I7000/i7010_chan.c` | 1 |
| `simulators/I7000/i701_chan.c` | 1 |
| `simulators/I7000/i7070_chan.c` | 1 |
| `simulators/I7000/i7070_defs.h` | 1 |
| `simulators/I7000/i7080_cpu.c` | 1 |
| `simulators/I7000/i7080_defs.h` | 1 |
| `simulators/I7000/i7090_chan.c` | 1 |
| `simulators/I7000/i7090_defs.h` | 1 |
| `simulators/Ibm1130/ibm1130_cpu.c` | 1 |
| `simulators/ND100/nd100_cpu.c` | 1 |
| `simulators/ND100/nd100_mm.c` | 1 |
| `simulators/PDP10/ka10_ch10.c` | 1 |
| `simulators/PDP10/ks10_cty.c` | 1 |
| `simulators/PDP10/ks10_uba.c` | 1 |
| `simulators/PDP10/pdp10_ksio.c` | 1 |
| `simulators/PDP11/pdp11_ch.c` | 1 |
| `simulators/PDP11/pdp11_cr.c` | 1 |
| `simulators/PDQ-3/pdq3_mem.c` | 1 |
| `simulators/PDQ-3/pdq3_stddev.c` | 1 |
| `simulators/SAGE/m68k_mem.c` | 1 |
| `simulators/SAGE/m68k_sys.c` | 1 |
| `simulators/VAX/vax4xx_rz94.c` | 1 |
| `simulators/VAX/vax4xx_va.c` | 1 |
| `simulators/VAX/vax4xx_ve.c` | 1 |
| `simulators/VAX/vax750_cmi.c` | 1 |
| `simulators/VAX/vax860_abus.c` | 1 |
| `simulators/VAX/vax_defs.h` | 1 |
| `simulators/VAX/vax_lk.c` | 1 |
| `simulators/VAX/vax_va.c` | 1 |
| `src/core/scp.c` | 1 |

### `-Wundef` (128 unique sites)

Example sites:

- `simulators/I7000/i7000_mt.c:437`: 'I7010' is not defined, evaluates to 0
- `simulators/I7000/i7000_mt.c:437`: 'I7080' is not defined, evaluates to 0
- `simulators/I7000/i7000_mt.c:473`: 'I7010' is not defined, evaluates to 0
- `simulators/I7000/i7000_mt.c:473`: 'I7080' is not defined, evaluates to 0
- `simulators/I7000/i7000_mt.c:494`: 'I7010' is not defined, evaluates to 0

| File | Unique sites |
|---|---:|
| `simulators/PDP10/kx10_sys.c` | 52 |
| `simulators/I7000/i7000_mt.c` | 36 |
| `simulators/PDP10/kx10_cpu.c` | 17 |
| `simulators/PDP10/kx10_defs.h` | 7 |
| `simulators/PDP10/ks10_kmc.c` | 2 |
| `simulators/PDP11/pdp11_kmc.c` | 2 |
| `simulators/I7000/i7090_sys.c` | 1 |
| `simulators/Ibm1130/ibm1130_cr.c` | 1 |
| `simulators/PDP10/ka10_dd.c` | 1 |
| `simulators/PDP10/ka10_dkb.c` | 1 |
| `simulators/PDP10/ka10_iii.c` | 1 |
| `simulators/PDP10/ka10_tv.c` | 1 |
| `simulators/PDP10/kx10_dpy.c` | 1 |
| `simulators/PDP11/pdp11_vh.c` | 1 |
| `simulators/SAGE/chip_defs.h` | 1 |
| `simulators/SEL32/sel32_ipu.c` | 1 |
| `simulators/SSEM/ssem_cpu.c` | 1 |
| `src/runtime/sim_card.c` | 1 |

### `-Wsign-compare` (120 unique sites)

Example sites:

- `simulators/3B2/3b2_ctc.c:305`: comparison of integers of different signs: 'int32' (aka 'int') and 'unsigned long'
- `simulators/3B2/3b2_scsi.c:841`: comparison of integers of different signs: 'int32' (aka 'int') and 'unsigned long'
- `simulators/ALTAIR/altair_sys.c:255`: comparison of integers of different signs: 'int32' (aka 'int') and 'unsigned long'
- `simulators/AltairZ80/altairz80_cpu.c:7014`: comparison of integers of different signs: 'ChipType' and 'const int32' (aka 'const int')
- `simulators/AltairZ80/altairz80_cpu.c:7246`: comparison of integers of different signs: 'int32' (aka 'int') and 'uint32' (aka 'unsigned int')

| File | Unique sites |
|---|---:|
| `third_party/slirp/socket.c` | 13 |
| `simulators/SEL32/sel32_ec.c` | 12 |
| `simulators/VAX/vax_cpu1.c` | 10 |
| `simulators/BESM6/besm6_mmu.c` | 7 |
| `simulators/I650/i650_mt.c` | 6 |
| `simulators/SAGE/m68k_sys.c` | 5 |
| `simulators/BESM6/besm6_sys.c` | 4 |
| `simulators/SAGE/m68k_cpu.c` | 4 |
| `simulators/VAX/vax4xx_va.c` | 4 |
| `third_party/slirp/tcp_input.c` | 3 |
| `simulators/AltairZ80/altairz80_cpu.c` | 2 |
| `simulators/PDP18B/pdp18b_g2tty.c` | 2 |
| `simulators/sigma/sigma_dp.c` | 2 |
| `simulators/swtp6800/common/dc-4.c` | 2 |
| `src/core/scp.c` | 2 |
| `src/runtime/sim_console.c` | 2 |
| `src/runtime/sim_scsi.c` | 2 |
| `src/runtime/sim_sock.c` | 2 |
| `src/runtime/sim_tmxr.c` | 2 |
| `third_party/slirp/ip_input.c` | 2 |
| `simulators/3B2/3b2_ctc.c` | 1 |
| `simulators/3B2/3b2_scsi.c` | 1 |
| `simulators/ALTAIR/altair_sys.c` | 1 |
| `simulators/AltairZ80/ibc.c` | 1 |
| `simulators/BESM6/besm6_mg.c` | 1 |
| `simulators/I1401/i1401_cd.c` | 1 |
| `simulators/I1620/i1620_tty.c` | 1 |
| `simulators/I650/i650_cpu.c` | 1 |
| `simulators/Intel-Systems/common/i8008.c` | 1 |
| `simulators/Intel-Systems/common/i8080.c` | 1 |
| `simulators/NOVA/eclipse_cpu.c` | 1 |
| `simulators/PDP11/pdp11_rh.c` | 1 |
| `simulators/PDP11/pdp11_rr.c` | 1 |
| `simulators/PDP11/pdp11_xq.c` | 1 |
| `simulators/PDP18B/pdp18b_dr15.c` | 1 |
| `simulators/PDP8/pdp8_ttx.c` | 1 |
| `simulators/SEL32/sel32_clk.c` | 1 |
| `simulators/SEL32/sel32_disk.c` | 1 |
| `simulators/SEL32/sel32_hsdp.c` | 1 |
| `simulators/VAX/vax730_stddev.c` | 1 |
| `simulators/VAX/vax750_stddev.c` | 1 |
| `simulators/VAX/vax780_stddev.c` | 1 |
| `simulators/VAX/vax820_stddev.c` | 1 |
| `simulators/VAX/vax860_stddev.c` | 1 |
| `simulators/VAX/vax_stddev.c` | 1 |
| `simulators/VAX/vax_va.c` | 1 |
| `simulators/linc/linc_sys.c` | 1 |
| `simulators/sigma/sigma_tt.c` | 1 |
| `third_party/slirp/sbuf.c` | 1 |
| `third_party/slirp/slirp.c` | 1 |
| `third_party/slirp/tftp.c` | 1 |
| `third_party/slirp/udp.c` | 1 |

### `-Wmissing-braces` (109 unique sites)

Example sites:

- `simulators/Intel-Systems/common/sys.c:160`: suggest braces around initialization of subobject
- `simulators/Intel-Systems/common/sys.c:161`: suggest braces around initialization of subobject
- `simulators/Intel-Systems/common/sys.c:162`: suggest braces around initialization of subobject
- `simulators/Intel-Systems/common/sys.c:163`: suggest braces around initialization of subobject
- `simulators/Intel-Systems/common/sys.c:164`: suggest braces around initialization of subobject

| File | Unique sites |
|---|---:|
| `simulators/Intel-Systems/common/sys.c` | 108 |
| `simulators/VAX/vax_uw.c` | 1 |

### `-Wunused-but-set-variable` (75 unique sites)

Example sites:

- `simulators/ALTAIR/altair_cpu.c:885`: variable 'bc' set but not used
- `simulators/ALTAIR/altair_cpu.c:955`: variable 'bc' set but not used
- `simulators/ALTAIR/altair_dsk.c:313`: variable 'rtn' set but not used
- `simulators/ALTAIR/altair_dsk.c:353`: variable 'rtn' set but not used
- `simulators/ALTAIR/altair_sys.c:186`: variable 'cflag' set but not used

| File | Unique sites |
|---|---:|
| `src/runtime/sim_scsi.c` | 8 |
| `src/runtime/sim_tmxr.c` | 6 |
| `simulators/I650/i650_cdp.c` | 5 |
| `simulators/Intel-Systems/common/isbc201.c` | 5 |
| `simulators/I650/i650_cdr.c` | 3 |
| `simulators/Intel-Systems/common/i8008.c` | 3 |
| `simulators/Intel-Systems/common/i8080.c` | 3 |
| `simulators/Intel-Systems/common/isbc202.c` | 3 |
| `simulators/Intel-Systems/common/isbc206.c` | 3 |
| `simulators/Intel-Systems/common/zx200a.c` | 3 |
| `simulators/ALTAIR/altair_cpu.c` | 2 |
| `simulators/ALTAIR/altair_dsk.c` | 2 |
| `simulators/ALTAIR/altair_sys.c` | 2 |
| `simulators/I650/i650_cpu.c` | 2 |
| `simulators/Ibm1130/ibm1130_cpu.c` | 2 |
| `simulators/VAX/vax4xx_rz80.c` | 2 |
| `simulators/VAX/vax820_ka.c` | 2 |
| `simulators/VAX/vax_va.c` | 2 |
| `simulators/BESM6/besm6_cpu.c` | 1 |
| `simulators/H316/h316_hi.c` | 1 |
| `simulators/I650/i650_sys.c` | 1 |
| `simulators/Intel-Systems/common/i8255.c` | 1 |
| `simulators/NOVA/eclipse_cpu.c` | 1 |
| `simulators/PDP10/ks10_dup.c` | 1 |
| `simulators/PDP10/ks10_dz.c` | 1 |
| `simulators/PDP10/ks10_lp.c` | 1 |
| `simulators/PDP10/kx10_rc.c` | 1 |
| `simulators/PDP10/pdp10_tu.c` | 1 |
| `simulators/PDP18B/pdp18b_cpu.c` | 1 |
| `simulators/S3/s3_disk.c` | 1 |
| `simulators/S3/s3_sys.c` | 1 |
| `simulators/VAX/vax820_mem.c` | 1 |
| `simulators/alpha/alpha_fpi.c` | 1 |
| `simulators/swtp6800/common/m6800.c` | 1 |
| `third_party/slirp/slirp.c` | 1 |

### `-Wformat-nonliteral` (55 unique sites)

Example sites:

- `simulators/3B2/3b2_ni.c:1034`: format string is not a string literal
- `simulators/3B2/3b2_ni.c:1035`: format string is not a string literal
- `simulators/3B2/3b2_ni.c:1036`: format string is not a string literal
- `simulators/3B2/3b2_ni.c:1037`: format string is not a string literal
- `simulators/3B2/3b2_ni.c:1038`: format string is not a string literal

| File | Unique sites |
|---|---:|
| `simulators/PDP11/pdp11_xq.c` | 12 |
| `simulators/PDP11/pdp11_xu.c` | 10 |
| `simulators/BESM6/besm6_sys.c` | 6 |
| `simulators/3B2/3b2_ni.c` | 5 |
| `simulators/Ibm1130/ibm1130_cpu.c` | 5 |
| `src/core/scp.c` | 4 |
| `simulators/PDP11/pdp11_io_lib.c` | 3 |
| `src/runtime/sim_ether.c` | 3 |
| `third_party/slirp_glue/glib_qemu_stubs.c` | 2 |
| `simulators/AltairZ80/m68k/m68kfpu.c` | 1 |
| `simulators/HP2100/hp2100_sys.c` | 1 |
| `simulators/HP3000/hp3000_sys.c` | 1 |
| `simulators/SAGE/m68k_scp.c` | 1 |
| `src/runtime/sim_tmxr.c` | 1 |

### `-Wunused-function` (35 unique sites)

Example sites:

- `simulators/BESM6/besm6_mg.c:344`: unused function 'log_data'
- `simulators/BESM6/besm6_pl.c:125`: unused function 'unicode_to_upp'
- `simulators/CDC1700/cdc1700_dc.c:303`: unused function 'description'
- `simulators/Intel-Systems/common/ioc-cont.c:94`: unused function 'ioc_cont_desc'
- `simulators/Intel-Systems/common/ipc-cont.c:53`: unused function 'ipc_cont_desc'

| File | Unique sites |
|---|---:|
| `simulators/PDP11/pdp11_ddcmp.h` | 10 |
| `simulators/PDP10/ks10_dup.c` | 2 |
| `simulators/PDP10/ks10_kmc.c` | 2 |
| `simulators/PDP11/pdp11_dup.c` | 2 |
| `src/runtime/sim_sock.c` | 2 |
| `src/runtime/sim_timer.c` | 2 |
| `third_party/slirp/slirp.c` | 2 |
| `simulators/BESM6/besm6_mg.c` | 1 |
| `simulators/BESM6/besm6_pl.c` | 1 |
| `simulators/CDC1700/cdc1700_dc.c` | 1 |
| `simulators/Intel-Systems/common/ioc-cont.c` | 1 |
| `simulators/Intel-Systems/common/ipc-cont.c` | 1 |
| `simulators/PDP10/ka10_auxcpu.c` | 1 |
| `simulators/PDP10/pdp6_slave.c` | 1 |
| `simulators/imlac/imlac_crt.c` | 1 |
| `simulators/imlac/imlac_kbd.c` | 1 |
| `simulators/linc/linc_crt.c` | 1 |
| `simulators/linc/linc_kbd.c` | 1 |
| `simulators/tt2500/tt2500_key.c` | 1 |
| `src/runtime/sim_console.c` | 1 |

### `-Wunused-variable` (31 unique sites)

Example sites:

- `simulators/3B2/3b2_cpu.c:884`: unused variable 'len'
- `simulators/BESM6/besm6_disk.c:474`: unused variable 'cnum'
- `simulators/BESM6/besm6_disk.c:496`: unused variable 'cnum'
- `simulators/BESM6/besm6_disk.c:521`: unused variable 'cnum'
- `simulators/BESM6/besm6_disk.c:564`: unused variable 'cnum'

| File | Unique sites |
|---|---:|
| `simulators/BESM6/besm6_disk.c` | 6 |
| `simulators/PDP10/ks10_dup.c` | 3 |
| `simulators/BESM6/besm6_mg.c` | 2 |
| `simulators/Intel-Systems/common/i8080.c` | 2 |
| `simulators/swtp6800/common/dc-4.c` | 2 |
| `src/runtime/sim_ether.c` | 2 |
| `simulators/3B2/3b2_cpu.c` | 1 |
| `simulators/Intel-Systems/common/isbc202.c` | 1 |
| `simulators/Intel-Systems/common/isbc208.c` | 1 |
| `simulators/PDP10/kx10_tym.c` | 1 |
| `simulators/PDP11/pdp11_mb.c` | 1 |
| `simulators/PDP8/pdp8_td.c` | 1 |
| `simulators/VAX/vax_gpx.c` | 1 |
| `simulators/VAX/vax_lk.c` | 1 |
| `simulators/VAX/vax_va.c` | 1 |
| `simulators/VAX/vax_vc.c` | 1 |
| `simulators/VAX/vax_vs.c` | 1 |
| `simulators/imlac/imlac_sys.c` | 1 |
| `simulators/swtp6800/common/m6800.c` | 1 |
| `src/runtime/sim_disk.c` | 1 |

### `-Wbitwise-instead-of-logical` (5 unique sites)

Example sites:

- `simulators/3B2/3b2_cpu.c:2593`: use of bitwise '|' with boolean operands
- `simulators/3B2/3b2_cpu.c:2598`: use of bitwise '|' with boolean operands
- `simulators/3B2/3b2_cpu.c:3580`: use of bitwise '&' with boolean operands
- `simulators/3B2/3b2_cpu.c:3769`: use of bitwise '&' with boolean operands
- `simulators/3B2/3b2_cpu.c:3779`: use of bitwise '&' with boolean operands

| File | Unique sites |
|---|---:|
| `simulators/3B2/3b2_cpu.c` | 5 |

### `-Wmisleading-indentation` (5 unique sites)

Example sites:

- `simulators/Ibm1130/ibm1130_cpu.c:1237`: misleading indentation; statement is not part of the previous 'if'
- `simulators/NOVA/nova_cpu.c:986`: misleading indentation; statement is not part of the previous 'if'
- `simulators/PDP18B/pdp18b_cpu.c:932`: misleading indentation; statement is not part of the previous 'if'
- `simulators/VAX/vax4xx_rz94.c:775`: misleading indentation; statement is not part of the previous 'if'
- `simulators/sigma/sigma_dk.c:431`: misleading indentation; statement is not part of the previous 'if'

| File | Unique sites |
|---|---:|
| `simulators/Ibm1130/ibm1130_cpu.c` | 1 |
| `simulators/NOVA/nova_cpu.c` | 1 |
| `simulators/PDP18B/pdp18b_cpu.c` | 1 |
| `simulators/VAX/vax4xx_rz94.c` | 1 |
| `simulators/sigma/sigma_dk.c` | 1 |

### `-Wsometimes-uninitialized` (2 unique sites)

Example sites:

- `simulators/I650/i650_cpu.c:1568`: variable 'neg' is used uninitialized whenever '&&' condition is false
- `simulators/I650/i650_cpu.c:1568`: variable 'neg' is used uninitialized whenever 'if' condition is false

| File | Unique sites |
|---|---:|
| `simulators/I650/i650_cpu.c` | 2 |

### `-Wstring-concatenation` (2 unique sites)

Example sites:

- `simulators/PDP10/pdp10_tu.c:337`: suspicious concatenation of string literals in an array initialization; did you mean to separate the elements with a comma?
- `simulators/PDP11/pdp11_tu.c:242`: suspicious concatenation of string literals in an array initialization; did you mean to separate the elements with a comma?

| File | Unique sites |
|---|---:|
| `simulators/PDP10/pdp10_tu.c` | 1 |
| `simulators/PDP11/pdp11_tu.c` | 1 |

### `-Wchar-subscripts` (2 unique sites)

Example sites:

- `simulators/S3/s3_cd.c:328`: array subscript is of type 'char'
- `simulators/S3/s3_cd.c:349`: array subscript is of type 'char'

| File | Unique sites |
|---|---:|
| `simulators/S3/s3_cd.c` | 2 |

### `-Wcast-function-type-mismatch` (1 unique sites)

Example sites:

- `src/runtime/sim_sock.c:892`: cast from 'int (*)(const struct sockaddr *restrict, socklen_t, char *restrict, socklen_t, char *restrict, socklen_t, int)' (aka 'int (*)(const struct sockaddr *restrict, unsigned int, char *restrict, unsigned int, char *restrict, unsigned int, int)') to 'getnameinfo_func' (aka 'int (*)(const struct sockaddr *, unsigned int, char *, unsigned long, char *, unsigned long, int)') converts to incompatible function type

| File | Unique sites |
|---|---:|
| `src/runtime/sim_sock.c` | 1 |

### `-Wuninitialized` (1 unique sites)

Example sites:

- `simulators/SEL32/sel32_ipu.c:6677`: variable 'tvl' is uninitialized when used here

| File | Unique sites |
|---|---:|
| `simulators/SEL32/sel32_ipu.c` | 1 |

