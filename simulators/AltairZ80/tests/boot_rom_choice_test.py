#!/usr/bin/env python3

import argparse
import re
import subprocess
from pathlib import Path


def write_sized_file(path: Path, size: int) -> None:
    chunk = b"x" * 32768
    remaining = size

    with path.open("wb") as image:
        while remaining >= len(chunk):
            image.write(chunk)
            remaining -= len(chunk)
        if remaining:
            image.write(chunk[:remaining])


def run_boot_rom_case(
    simulator: Path,
    work_dir: Path,
    name: str,
    image_size: int,
    expected_bytes: tuple[str, str, str],
) -> None:
    image = work_dir / f"{name}.dsk"
    script = work_dir / f"{name}.ini"

    write_sized_file(image, image_size)
    script.write_text(
        "\n".join(
            [
                "set cpu 8080",
                "set cpu altairrom",
                "break 0xff00",
                f'attach dsk0 "{image}"',
                "boot dsk0",
                "examine 0xff56-0xff58",
                "exit 0",
                "",
            ]
        ),
        encoding="utf-8",
    )

    result = subprocess.run(
        [str(simulator), str(script), "-v"],
        check=False,
        text=True,
        capture_output=True,
        timeout=10,
    )
    if result.returncode != 0:
        raise RuntimeError(
            f"{name}: simulator exited with {result.returncode}\n"
            f"stdout:\n{result.stdout}\n"
            f"stderr:\n{result.stderr}"
        )

    for address, expected_byte in zip(("FF56", "FF57", "FF58"), expected_bytes):
        pattern = rf"{address}:\s+{re.escape(expected_byte)}\b"
        if re.search(pattern, result.stdout) is None:
            raise AssertionError(
                f"{name}: expected {address} to be {expected_byte}\n"
                f"stdout:\n{result.stdout}"
            )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("simulator", type=Path)
    parser.add_argument("--work-dir", type=Path, required=True)
    args = parser.parse_args()

    args.work_dir.mkdir(parents=True, exist_ok=True)
    run_boot_rom_case(
        args.simulator, args.work_dir, "mini-disk", 76720, ("00", "3E", "10")
    )
    run_boot_rom_case(
        args.simulator, args.work_dir, "regular-altair", 337664, ("06", "00", "C5")
    )
    run_boot_rom_case(
        args.simulator, args.work_dir, "nonstandard", 1000, ("06", "08", "C5")
    )


if __name__ == "__main__":
    main()
