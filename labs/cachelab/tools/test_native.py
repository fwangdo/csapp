#!/usr/bin/env python3
"""Native Cache Lab checks. Student solutions and supplied binaries stay untouched."""

import argparse
from pathlib import Path
import random
import re
import subprocess
import sys
import tempfile

from cache_oracle import simulate, summary

ROOT = Path(__file__).resolve().parent.parent
SUMMARY = re.compile(r"hits:(\d+)\s+misses:(\d+)\s+evictions:(\d+)")

# Public examples from the CS:APP Cache Lab writeup, section 6.1.
# https://csapp.cs.cmu.edu/2e/cachelab.pdf
PUBLIC_CASES = [
    (1, 1, 1, "yi2.trace", (9, 8, 6)),
    (4, 2, 4, "yi.trace", (4, 5, 2)),
    (2, 1, 4, "dave.trace", (2, 3, 1)),
    (2, 1, 3, "trans.trace", (167, 71, 67)),
    (2, 2, 3, "trans.trace", (201, 37, 29)),
    (2, 4, 3, "trans.trace", (212, 26, 10)),
    (5, 1, 5, "trans.trace", (231, 7, 0)),
    (5, 1, 5, "long.trace", (265189, 21775, 21743)),
]

# Hand-calculated cases: no algorithm implementation is needed to check these.
SMALL_CASES = [
    ("empty", 1, 1, 1, "", (0, 0, 0)),
    ("ignore-instructions", 1, 1, 1, "I 0,1\nI 4,1\n", (0, 0, 0)),
    ("modify-twice", 1, 1, 1, " M 0,1\n M 0,1\n", (3, 1, 0)),
    ("block-offset", 1, 1, 3, " L 0,1\n S 7,1\n L 8,1\n", (1, 2, 0)),
    ("lru-hit-refresh", 1, 2, 1,
     " L 0,1\n L 4,1\n L 0,1\n L 8,1\n L 4,1\n", (1, 4, 2)),
    ("64-bit-tags", 1, 1, 1,
     " L 0,1\n L 100000000,1\n M 8000000000000000,1\n L 0,1\n",
     (1, 4, 3)),
]


def random_trace(s, E, b, seed, count=300):
    rng = random.Random(seed)
    # Hot blocks, conflicting sets, and tags using both halves of a 64-bit address.
    pool = [rng.randrange(1 << s) + ((i + (1 << (40 - s - b))) << s)
            for i in range(4 * E + 8)]
    pool += [i << s for i in range(3 * E + 1)]
    return "".join(f"{'I ' if op == 'I' else ' ' + op} {address:x},1\n"
                   for op, address in (
                       (rng.choice("ILSSMM"),
                        (rng.choice(pool) << b) + rng.randrange(1 << b))
                       for _ in range(count)))


def check_csim(binary, trace, params, expected, directory):
    s, E, b = params
    result_file = directory / ".csim_results"
    result_file.unlink(missing_ok=True)
    result = subprocess.run([str(binary), "-s", str(s), "-E", str(E), "-b", str(b),
                             "-t", str(trace)], cwd=directory, capture_output=True,
                            text=True, timeout=15)
    match = SUMMARY.search(result.stdout)
    actual = tuple(map(int, match.groups())) if match else None
    if result.returncode:
        return False, f"exit {result.returncode}: {result.stderr.strip()}"
    if actual != expected:
        return False, f"expected {summary(expected)}; got {summary(actual) if actual else result.stdout.strip()!r}"
    # printSummary must also produce this file. Each test runs in a fresh context.
    try:
        saved = tuple(map(int, result_file.read_text().split()))
    except (OSError, ValueError):
        saved = None
    if saved != expected:
        return False, ".csim_results missing or inconsistent; use printSummary"
    return True, summary(actual)


def test_csim(args):
    binary = args.binary.resolve()
    if not binary.is_file():
        raise ValueError(f"missing {binary}; run make csim first")
    passed = total = 0
    with tempfile.TemporaryDirectory(prefix="cachelab-a-") as name:
        directory = Path(name)
        cases = []
        for s, E, b, filename, expected in PUBLIC_CASES:
            trace = ROOT / "traces" / filename
            with trace.open() as stream:
                observed = simulate(stream, s, E, b)
            if observed != expected:
                raise ValueError(f"oracle/public fixture mismatch: {filename}: {observed} != {expected}")
            cases.append((filename, (s, E, b), trace, expected))
        for i, (label, s, E, b, text, expected) in enumerate(SMALL_CASES):
            trace = directory / f"small-{i}.trace"
            trace.write_text(text)
            if simulate(text.splitlines(), s, E, b) != expected:
                raise ValueError(f"oracle/hand-calculated fixture mismatch: {label}")
            cases.append((label, (s, E, b), trace, expected))
        for config in [(1, 1, 1), (2, 2, 3), (3, 4, 5), (4, 8, 4), (5, 1, 5), (2, 3, 2)]:
            for seed in range(args.seed, args.seed + args.random_cases):
                text = random_trace(*config, seed)
                trace = directory / f"random-{len(cases)}.trace"
                trace.write_text(text)
                cases.append((f"random seed={seed}", config, trace,
                              simulate(text.splitlines(), *config)))
        for label, config, trace, expected in cases:
            try:
                ok, detail = check_csim(binary, trace, config, expected, directory)
            except subprocess.TimeoutExpired:
                ok, detail = False, "timed out after 15 seconds"
            total += 1
            passed += ok
            print(f"{'PASS' if ok else 'FAIL'} {label} (s,E,b)={config}: {detail}")
            if not ok and args.fail_fast:
                break
    print(f"Part A: {passed}/{total} passed (independent oracle, not official grading)")
    return 0 if passed == total else 1


def build_trans(directory, source, clang):
    obj, executable = directory / "trans.o", directory / "native-trans"
    common = [clang, "-std=c99", "-g", "-O0", "-Wall", "-Werror",
              "-fsanitize=address", "-I", str(ROOT)]
    subprocess.run(common + ["-fsanitize-coverage=trace-pc,trace-loads,trace-stores",
                             "-c", str(source), "-o", str(obj)], check=True, timeout=60)
    subprocess.run(common + [str(ROOT / "tools/native_trans.c"), str(ROOT / "cachelab.c"),
                             str(obj), "-o", str(executable)], check=True, timeout=60)
    return executable


def run_trans(executable, M, N, fn, seed, trace, b_base, directory):
    result = subprocess.run([str(executable), str(M), str(N), str(fn), str(seed),
                             str(trace), str(b_base)], cwd=directory,
                            capture_output=True, text=True, timeout=15)
    if result.returncode:
        return False, (result.stderr.strip() or result.stdout.strip())[-4000:], None
    match = re.search(r"^NATIVE_TRANS_RESULT correct=1 accesses=(\d+)$", result.stdout, re.M)
    if not match:
        return False, "missing harness result", None
    with trace.open() as stream:
        counts = simulate(stream, 5, 1, 5)
    if counts[0] + counts[1] != int(match[1]):
        return False, "trace/access counter mismatch", None
    return True, summary(counts), counts


def test_trans(args):
    if args.trials < 1 or args.seed < 1 or args.seed + args.trials > 1 << 32:
        raise ValueError("require trials >= 1 and nonzero 32-bit seeds")
    if args.b_offset < 256 * 256 * 4 or args.b_offset % 4 or args.b_offset > (1 << 64) - 256 * 256 * 4:
        raise ValueError("b-offset must be a multiple of 4, >= 262144, within 64-bit addresses")
    shapes = []
    for shape in args.sizes:
        match = re.fullmatch(r"(\d+)[xX](\d+)", shape)
        if not match or not all(1 <= int(v) <= 256 for v in match.groups()):
            raise ValueError(f"invalid MxN size: {shape}")
        shapes.append(tuple(map(int, match.groups())))
    if args.trace_dir:
        args.trace_dir = args.trace_dir.resolve()
        args.trace_dir.mkdir(parents=True, exist_ok=True)
    print("Part B: Clang -O0 array-only model; s=5 E=1 b=5; "
          f"A=0 B={args.b_offset:#x}; NOT an official miss count", flush=True)
    passed = total = 0
    with tempfile.TemporaryDirectory(prefix="cachelab-b-") as name:
        directory = Path(name)
        executable = build_trans(directory, args.source.resolve(), args.clang)
        for M, N in shapes:
            for seed in range(args.seed, args.seed + args.trials):
                trace = (args.trace_dir or directory) / f"f{args.function}-{M}x{N}-seed{seed}.trace"
                try:
                    ok, detail, _ = run_trans(executable, M, N, args.function, seed,
                                              trace, args.b_offset, directory)
                except subprocess.TimeoutExpired:
                    ok, detail = False, "timed out after 15 seconds"
                total += 1
                passed += ok
                print(f"{'PASS' if ok else 'FAIL'} {M}x{N} function={args.function} seed={seed}: {detail}")
    print(f"Part B correctness: {passed}/{total} passed")
    return 0 if passed == total else 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    part_a = commands.add_parser("csim", help="compare csim with public fixtures and independent oracle")
    part_a.add_argument("--binary", type=Path, default=ROOT / "csim")
    part_a.add_argument("--seed", type=int, default=1)
    part_a.add_argument("--random-cases", type=int, default=4, help="cases per cache configuration")
    part_a.add_argument("--fail-fast", action="store_true")
    part_a.set_defaults(run=test_csim)
    part_b = commands.add_parser("trans", help="validate and instrument unmodified trans.c")
    part_b.add_argument("--source", type=Path, default=ROOT / "trans.c")
    part_b.add_argument("--clang", default="clang")
    part_b.add_argument("--function", type=int, default=0)
    part_b.add_argument("--sizes", nargs="+", default=["32x32", "64x64", "61x67"])
    part_b.add_argument("--trials", type=int, default=3)
    part_b.add_argument("--seed", type=int, default=1)
    part_b.add_argument("--b-offset", type=lambda x: int(x, 0), default=262144)
    part_b.add_argument("--trace-dir", type=Path, help="keep traces for oracle -v debugging")
    part_b.set_defaults(run=test_trans)
    args = parser.parse_args()
    if args.command == "csim" and args.random_cases < 0:
        parser.error("random-cases must be >= 0")
    try:
        return args.run(args)
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
