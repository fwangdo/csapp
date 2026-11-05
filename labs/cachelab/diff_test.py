#!/usr/bin/env python3
"""Build csim and compare its results with the independent Python oracle."""

import argparse
from pathlib import Path
import subprocess
import sys


ROOT = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / "tools"))

from test_native import test_csim


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--seed", type=int, default=1,
                        help="first random seed (default: 1)")
    parser.add_argument("--random-cases", type=int, default=20,
                        help="random cases per cache configuration (default: 20)")
    parser.add_argument("--fail-fast", action="store_true",
                        help="stop at the first failure")
    parser.add_argument("--binary", type=Path,
                        help="test an existing binary instead of building csim")
    args = parser.parse_args()
    if args.random_cases < 0:
        parser.error("--random-cases must be >= 0")

    try:
        if args.binary is None:
            print("Building csim...", flush=True)
            subprocess.run(["make", "csim"], cwd=ROOT, check=True, timeout=60)
            args.binary = ROOT / "csim"
        print(f"Comparing csim with Python oracle (seed={args.seed}, "
              f"random cases per configuration={args.random_cases})", flush=True)
        return test_csim(args)
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
