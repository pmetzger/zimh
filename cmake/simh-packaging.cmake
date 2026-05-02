##
## Update this file as simulator package-family metadata changes.
## Keep CPack component declarations and other packaging-specific metadata
## here. Do not regenerate this file from the legacy generator.

## The default runtime support component/family:
cpack_add_component(runtime_support
    DISPLAY_NAME "Runtime support"
    DESCRIPTION "Required SIMH runtime support (documentation, shared libraries)"
    REQUIRED
)

## Current Markdown documentation for ZIMH.  The inherited Word-format
## source material is retained in the repository but is not installed.
install(DIRECTORY docs/
    TYPE DOC
    COMPONENT runtime_support
    PATTERN "legacy-word" EXCLUDE
)

cpack_add_component(altairz80_family
    DISPLAY_NAME "Altair Z80 simulator."
    DESCRIPTION "The Altair Z80 simulator with M68000 support. Simulators: zimh-altairz80"
)
cpack_add_component(att3b2_family
    DISPLAY_NAME "ATT&T 3b2 collection"
    DESCRIPTION "The AT&T 3b2 simulator family. Simulators: zimh-3b2-400, zimh-3b2-700"
)
cpack_add_component(b5500_family
    DISPLAY_NAME "Burroughs 5500"
    DESCRIPTION "The Burroughs 5500 system simulator. Simulators: zimh-b5500"
)
cpack_add_component(cdc1700_family
    DISPLAY_NAME "CDC 1700"
    DESCRIPTION "The Control Data Corporation's systems. Simulators: zimh-cdc1700"
)
cpack_add_component(decpdp_family
    DISPLAY_NAME "DEC PDP family"
    DESCRIPTION "Digital Equipment Corporation PDP systems. Simulators: zimh-pdp1, zimh-pdp15, zimh-pdp4, zimh-pdp6, zimh-pdp7, zimh-pdp8, zimh-pdp9"
)
cpack_add_component(default_family
    DISPLAY_NAME "Default SIMH simulator family."
    DESCRIPTION "The SIMH simulator collection of historical processors and computing systems that do not belong to
any other simulated system family. Simulators: zimh-altair, zimh-besm6, zimh-linc, zimh-ssem, zimh-tt2500, zimh-tx-0"
)
cpack_add_component(dgnova_family
    DISPLAY_NAME "DG Nova and Eclipse"
    DESCRIPTION "Data General NOVA and Eclipse system simulators. Simulators: zimh-eclipse, zimh-nova"
)
cpack_add_component(experimental_family
    DISPLAY_NAME "Experimental (work-in-progress) simulators"
    DESCRIPTION "Experimental or work-in-progress simulators not in the SIMH mainline simulator suite. Simulators: zimh-alpha, zimh-pdq3, zimh-sage"
)
cpack_add_component(gould_family
    DISPLAY_NAME "Gould simulators"
    DESCRIPTION "Gould Systems simulators. Simulators: zimh-sel32"
)
cpack_add_component(grisys_family
    DISPLAY_NAME "GRI Systems GRI-909"
    DESCRIPTION "GRI Systems GRI-909 system simulator. Simulators: zimh-gri"
)
cpack_add_component(honeywell_family
    DISPLAY_NAME "Honeywell H316"
    DESCRIPTION "Honeywell H-316 system simulator. Simulators: zimh-h316"
)
cpack_add_component(hp_family
    DISPLAY_NAME "HP 2100, 3000"
    DESCRIPTION "Hewlett-Packard H2100 and H3000 simulators. Simulators: zimh-hp2100, zimh-hp3000"
)
cpack_add_component(ibm_family
    DISPLAY_NAME "IBM"
    DESCRIPTION "IBM system simulators: i650. Simulators: zimh-i1401, zimh-i1620, zimh-i650, zimh-i701, zimh-i7010, zimh-i704, zimh-i7070, zimh-i7080, zimh-i7090, zimh-i7094, zimh-ibm1130, zimh-s3"
)
cpack_add_component(imlac_family
    DISPLAY_NAME "IMLAC"
    DESCRIPTION "IMLAC system simulators. Simulators: zimh-imlac"
)
cpack_add_component(intel_family
    DISPLAY_NAME "Intel"
    DESCRIPTION "Intel system simulators. Simulators: zimh-intel-mds, zimh-scelbi"
)
cpack_add_component(interdata_family
    DISPLAY_NAME "Interdata"
    DESCRIPTION "Interdata systems simulators. Simulators: zimh-id16, zimh-id32"
)
cpack_add_component(lgp_family
    DISPLAY_NAME "LGP"
    DESCRIPTION "Librascope systems. Simulators: zimh-lgp"
)
cpack_add_component(norsk_family
    DISPLAY_NAME "ND simulators"
    DESCRIPTION "Norsk Data systems simulator family. Simulators: zimh-nd100"
)
cpack_add_component(pdp10_family
    DISPLAY_NAME "DEC PDP-10 collection"
    DESCRIPTION "DEC PDP-10 architecture simulators and variants. Simulators: zimh-pdp10, zimh-pdp10-ka, zimh-pdp10-ki, zimh-pdp10-kl, zimh-pdp10-ks"
)
cpack_add_component(pdp11_family
    DISPLAY_NAME "DEC PDP-11 collection."
    DESCRIPTION "DEC PDP-11 and PDP-11-derived architecture simulators. Simulators: zimh-pdp11, zimh-uc15"
)
cpack_add_component(sds_family
    DISPLAY_NAME "SDS simulators"
    DESCRIPTION "Scientific Data Systems (SDS) systems. Simulators: zimh-sds, zimh-sigma"
)
cpack_add_component(swtp_family
    DISPLAY_NAME "SWTP simulators"
    DESCRIPTION "Southwest Technical Products (SWTP) system simulators. Simulators: zimh-swtp6800mp-a, zimh-swtp6800mp-a2"
)
cpack_add_component(vax_family
    DISPLAY_NAME "DEC VAX simulator collection"
    DESCRIPTION "The Digital Equipment Corporation VAX (plural: VAXen) simulator family. Simulators: zimh-infoserver100, zimh-infoserver1000, zimh-infoserver150vxt, zimh-microvax1, zimh-microvax2, zimh-microvax2000, zimh-microvax3100, zimh-microvax3100e, zimh-microvax3100m80, zimh-microvax3900, zimh-rtvax1000, zimh-vax, zimh-vax730, zimh-vax750, zimh-vax780, zimh-vax8200, zimh-vax8600, zimh-vaxstation3100m30, zimh-vaxstation3100m38, zimh-vaxstation3100m76, zimh-vaxstation4000m60, zimh-vaxstation4000vlc"
)
