#!/usr/bin/env python3
"""Build and test trans.c natively, including modeled cache statistics."""

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / "tools"))

from test_native import main


if __name__ == "__main__":
    sys.exit(main(["trans", *sys.argv[1:]]))
