#!/usr/bin/env python3

import argparse
import re
import subprocess
from pathlib import Path


def run_index_pulse_case(
    simulator: Path,
    work_dir: Path,
    name: str,
    control_byte: int,
    expected_queue_time: int,
) -> None:
    image = work_dir / f"{name}.dsk"
    script = work_dir / f"{name}.ini"

    script.write_text(
        "\n".join(
            [
                "set cpu 8080",
                "set cromfdc enabled",
                "set wd179x enabled",
                f'attach -n cromfdc0 "{image}"',
                "",
                "deposit 0 3e",
                f"deposit 1 {control_byte:02x}",
                "deposit 2 d3",
                "deposit 3 34",
                "",
                "deposit 4 3e",
                "deposit 5 d4",
                "deposit 6 d3",
                "deposit 7 30",
                "deposit 8 76",
                "",
                "go 0",
                "show queue",
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

    match = re.search(r"WD179X unit 0 at (\d+)\b", result.stdout)
    if match is None:
        raise AssertionError(
            f"{name}: WD179X index pulse was not queued\n"
            f"stdout:\n{result.stdout}"
        )

    actual_queue_time = int(match.group(1))
    if actual_queue_time != expected_queue_time:
        raise AssertionError(
            f"{name}: expected WD179X queue time {expected_queue_time}, "
            f"got {actual_queue_time}\nstdout:\n{result.stdout}"
        )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("simulator", type=Path)
    parser.add_argument("--work-dir", type=Path, required=True)
    args = parser.parse_args()

    args.work_dir.mkdir(parents=True, exist_ok=True)
    run_index_pulse_case(args.simulator, args.work_dir, "5-inch", 0x21, 58199)
    run_index_pulse_case(args.simulator, args.work_dir, "8-inch", 0x31, 48596)


if __name__ == "__main__":
    main()
