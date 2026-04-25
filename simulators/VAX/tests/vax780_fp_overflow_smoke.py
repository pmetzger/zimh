#!/usr/bin/env python3
"""Generate and run focused VAX-11/780 signed-overflow smoke tests."""

import argparse
import subprocess
import sys
import tempfile
from pathlib import Path


PC = "1000"


CASES = [
    {
        "name": "ASHQ left shift",
        "instruction": "ashq #1,r0,r2",
        "regs": {"r0": "40000000", "r1": "0"},
        "expect": {
            "PC": "1004",
            "R2": "80000000",
            "R3": "0",
            "PSL_CC": "0",
        },
    },
    {
        "name": "ASHQ left shift overflow",
        "instruction": "ashq #1,r0,r2",
        "regs": {"r0": "0", "r1": "40000000"},
        "expect": {
            "PC": "1004",
            "R2": "0",
            "R3": "80000000",
            "PSL_CC": "A",
        },
    },
    {
        "name": "ASHQ arithmetic right shift",
        "instruction": "ashq #-1,r0,r2",
        "regs": {"r0": "0", "r1": "80000000"},
        "expect": {
            "PC": "1005",
            "R2": "0",
            "R3": "C0000000",
            "PSL_CC": "8",
        },
    },
    {
        "name": "EDIV positive",
        "instruction": "ediv r0,r2,r4,r5",
        "regs": {"r0": "3", "r2": "10", "r3": "0"},
        "expect": {"PC": "1005", "R4": "5", "R5": "1", "PSL_CC": "0"},
    },
    {
        "name": "EDIV negative dividend",
        "instruction": "ediv r0,r2,r4,r5",
        "regs": {"r0": "3", "r2": "FFFFFFF6", "r3": "FFFFFFFF"},
        "expect": {
            "PC": "1005",
            "R4": "FFFFFFFD",
            "R5": "FFFFFFFF",
            "PSL_CC": "8",
        },
    },
    {
        "name": "EDIV negative divisor",
        "instruction": "ediv r0,r2,r4,r5",
        "regs": {"r0": "FFFFFFFD", "r2": "10", "r3": "0"},
        "expect": {
            "PC": "1005",
            "R4": "FFFFFFFB",
            "R5": "1",
            "PSL_CC": "8",
        },
    },
    {
        "name": "EDIV INT_MIN divisor",
        "instruction": "ediv r0,r2,r4,r5",
        "regs": {"r0": "80000000", "r2": "80000000", "r3": "FFFFFFFF"},
        "expect": {"PC": "1005", "R4": "1", "R5": "0", "PSL_CC": "0"},
    },
    {
        "name": "EDIV INT64_MIN dividend",
        "instruction": "ediv r0,r2,r4,r5",
        "regs": {"r0": "1", "r2": "0", "r3": "80000000"},
        "expect": {"PC": "1005", "R4": "0", "R5": "0", "PSL_CC": "6"},
    },
    {
        "name": "EDIV exact INT_MIN quotient",
        "instruction": "ediv r0,r2,r4,r5",
        "regs": {"r0": "FFFFFFFF", "r2": "80000000", "r3": "0"},
        "expect": {
            "PC": "1005",
            "R4": "80000000",
            "R5": "0",
            "PSL_CC": "8",
        },
    },
    {
        "name": "EDIV positive quotient overflow",
        "instruction": "ediv r0,r2,r4,r5",
        "regs": {"r0": "1", "r2": "80000000", "r3": "0"},
        "expect": {
            "PC": "1005",
            "R4": "80000000",
            "R5": "0",
            "PSL_CC": "A",
        },
    },
    {
        "name": "CVTLF positive",
        "instruction": "cvtlf #1,r0",
        "expect": {"PC": "1003", "R0": "4080", "PSL_CC": "0"},
    },
    {
        "name": "CVTLF negative",
        "instruction": "cvtlf #-1,r0",
        "expect": {"PC": "1007", "R0": "C080", "PSL_CC": "8"},
    },
    {
        "name": "CVTLF LONG_MIN",
        "instruction": "cvtlf r0,r1",
        "regs": {"r0": "80000000"},
        "expect": {"PC": "1003", "R1": "D000", "PSL_CC": "8"},
    },
    {
        "name": "CVTLD LONG_MIN",
        "instruction": "cvtld r0,r2",
        "regs": {"r0": "80000000"},
        "expect": {
            "PC": "1003",
            "R2": "D000",
            "R3": "0",
            "PSL_CC": "8",
        },
    },
    {
        "name": "CVTLG LONG_MIN",
        "instruction": "cvtlg r0,r2",
        "regs": {"r0": "80000000"},
        "expect": {
            "PC": "1004",
            "R2": "C200",
            "R3": "0",
            "PSL_CC": "8",
        },
    },
    {
        "name": "CVTLH LONG_MIN",
        "instruction": "cvtlh r0,r2",
        "regs": {"r0": "80000000"},
        "expect": {
            "PC": "1004",
            "R2": "C020",
            "R3": "0",
            "R4": "0",
            "R5": "0",
            "PSL_CC": "8",
        },
    },
    {
        "name": "ADDF effective subtraction",
        "instruction": "addf3 r0,r1,r2",
        "regs": {"r0": "4280", "r1": "C080"},
        "expect": {"PC": "1004", "R2": "4270", "PSL_CC": "0"},
    },
    {
        "name": "ADDD effective subtraction",
        "instruction": "addd3 r0,r2,r4",
        "regs": {"r0": "4280", "r1": "0", "r2": "C080", "r3": "0"},
        "expect": {
            "PC": "1004",
            "R4": "4270",
            "R5": "0",
            "PSL_CC": "0",
        },
    },
    {
        "name": "ADDG effective subtraction",
        "instruction": "addg3 r0,r2,r4",
        "regs": {"r0": "4050", "r1": "0", "r2": "C010", "r3": "0"},
        "expect": {
            "PC": "1005",
            "R4": "404E",
            "R5": "0",
            "PSL_CC": "0",
        },
    },
    {
        "name": "EMODF integral result",
        "instruction": "emodf r0,#0,r1,r2,r3",
        "regs": {"r0": "41A0", "r1": "4080"},
        "expect": {"PC": "1006", "R2": "5", "R3": "0", "PSL_CC": "4"},
    },
    {
        "name": "EMODF negative integral result",
        "instruction": "emodf r0,#0,r1,r2,r3",
        "regs": {"r0": "C1A0", "r1": "4080"},
        "expect": {
            "PC": "1006",
            "R2": "FFFFFFFB",
            "R3": "0",
            "PSL_CC": "4",
        },
    },
    {
        "name": "EMODF negative overflow",
        "instruction": "emodf r0,#0,r1,r2,r3",
        "regs": {"r0": "D400", "r1": "4080"},
        "expect": {"PC": "1006", "R2": "0", "R3": "0", "PSL_CC": "6"},
    },
    {
        "name": "EMODD negative integral result",
        "instruction": "emodd r0,#0,r2,r4,r6",
        "regs": {"r0": "C1A0", "r1": "0", "r2": "4080", "r3": "0"},
        "expect": {
            "PC": "1006",
            "R4": "FFFFFFFB",
            "R6": "0",
            "R7": "0",
            "PSL_CC": "4",
        },
    },
    {
        "name": "EMODD negative overflow",
        "instruction": "emodd r0,#0,r2,r4,r6",
        "regs": {"r0": "D400", "r1": "0", "r2": "4080", "r3": "0"},
        "expect": {
            "PC": "1006",
            "R4": "0",
            "R6": "0",
            "R7": "0",
            "PSL_CC": "6",
        },
    },
    {
        "name": "EMODG negative integral result",
        "instruction": "emodg r0,#0,r2,r4,r6",
        "regs": {"r0": "C034", "r1": "0", "r2": "4010", "r3": "0"},
        "expect": {
            "PC": "1007",
            "R4": "FFFFFFFB",
            "R6": "0",
            "R7": "0",
            "PSL_CC": "4",
        },
    },
    {
        "name": "EMODG negative overflow",
        "instruction": "emodg r0,#0,r2,r4,r6",
        "regs": {"r0": "C500", "r1": "0", "r2": "4010", "r3": "0"},
        "expect": {
            "PC": "1007",
            "R4": "0",
            "R6": "0",
            "R7": "0",
            "PSL_CC": "6",
        },
    },
    {
        "name": "EMODH negative integral result",
        "instruction": "emodh r0,#0,r4,r8,r10",
        "regs": {
            "r0": "4000C003",
            "r1": "0",
            "r2": "0",
            "r3": "0",
            "r4": "4001",
            "r5": "0",
            "r6": "0",
            "r7": "0",
        },
        "expect": {
            "PC": "1007",
            "R8": "FFFFFFFB",
            "R10": "0",
            "R11": "0",
            "R12": "0",
            "R13": "0",
            "PSL_CC": "4",
        },
    },
    {
        "name": "EMODH negative overflow",
        "instruction": "emodh r0,#0,r4,r8,r10",
        "regs": {
            "r0": "C028",
            "r1": "0",
            "r2": "0",
            "r3": "0",
            "r4": "4001",
            "r5": "0",
            "r6": "0",
            "r7": "0",
        },
        "expect": {
            "PC": "1007",
            "R8": "0",
            "R10": "0",
            "R11": "0",
            "R12": "0",
            "R13": "0",
            "PSL_CC": "6",
        },
    },
]


def emit_assert(lines, name, value):
    if name == "PSL_CC":
        lines.append(f"assert ((PSL & 0xF) == 0x{value})")
    else:
        lines.append(f"assert {name}={value}")


def generate_ini():
    lines = ["set noverify", "set cpu 1m", ""]
    for case in CASES:
        lines.extend(
            [
                "reset",
                f"echo Smoke: {case['name']}",
                f"deposit -m {PC} {case['instruction']}",
            ]
        )
        for name, value in case.get("regs", {}).items():
            lines.append(f"deposit {name} {value}")
        lines.extend([f"deposit pc {PC}", "step 1"])
        for name, value in case["expect"].items():
            emit_assert(lines, name, value)
        lines.append("")
    lines.extend(["echo VAX780 floating/integer helper smoke passed", "exit"])
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("simulator", type=Path)
    parser.add_argument("--work-dir", type=Path)
    args = parser.parse_args()

    if not args.simulator.exists():
        print(f"simulator not found: {args.simulator}", file=sys.stderr)
        return 2

    ini_text = generate_ini()
    with tempfile.TemporaryDirectory(dir=args.work_dir) as tmpdir:
        ini_path = Path(tmpdir) / "vax780-fp-overflow-smoke.ini"
        ini_path.write_text(ini_text, encoding="utf-8")
        result = subprocess.run(
            [str(args.simulator), "-q", str(ini_path)],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
        )

    if result.returncode != 0:
        print("Generated simulator command file:", file=sys.stderr)
        print(ini_text, file=sys.stderr)
        print("Simulator stdout:", file=sys.stderr)
        print(result.stdout, file=sys.stderr)
        print("Simulator stderr:", file=sys.stderr)
        print(result.stderr, file=sys.stderr)
        return result.returncode

    print(result.stdout, end="")
    if result.stderr:
        print(result.stderr, end="", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
