"""Checks for the testing tools, independent of the student's unfinished answers."""

import io
import itertools
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from cache_oracle import Cache, records, simulate
from test_native import (PUBLIC_CASES, ROOT, SMALL_CASES, build_trans,
                         random_trace, run_trans)


class OracleChecks(unittest.TestCase):
    def test_public_results(self):
        for s, E, b, name, expected in PUBLIC_CASES:
            with self.subTest(trace=name, config=(s, E, b)):
                with (ROOT / "traces" / name).open() as trace:
                    self.assertEqual(simulate(trace, s, E, b), expected)

    def test_hand_calculated_results(self):
        for name, s, E, b, text, expected in SMALL_CASES:
            with self.subTest(case=name):
                self.assertEqual(simulate(text.splitlines(), s, E, b), expected)

    def test_lru_against_history(self):
        # Independent check: derive expected events from the full access history
        # (reuse distance), without maintaining a cache or replacement state.
        for s, E, b in [(0, 1, 0), (0, 2, 0), (0, 3, 2), (1, 1, 1), (1, 2, 2)]:
            for length in range(1, 6):
                for sequence in itertools.product(range(4), repeat=length):
                    history, expected = [], [0, 0, 0]
                    cache = Cache(s, E, b)
                    for block in sequence:
                        previous = []
                        for old in reversed(history):
                            if old % (1 << s) == block % (1 << s) and old not in previous:
                                previous.append(old)
                        if block in previous[:E]:
                            expected[0] += 1
                        else:
                            expected[1] += 1
                            expected[2] += len(previous) >= E
                        history.append(block)
                        cache.access(block << b)
                    self.assertEqual(cache.counts, tuple(expected))

    def test_verbose_modify_and_instruction(self):
        output = io.StringIO()
        self.assertEqual(simulate(["I 0,1", " M 0,1", " M 4,1"], 1, 1, 1, output),
                         (2, 2, 1))
        self.assertEqual(output.getvalue(), "M 0,1 miss hit\nM 4,1 miss eviction hit\n")

    def test_input_validation(self):
        for text in [" L 10000000000000000,1", " S 0,0", " X 0,1", " L -1,1"]:
            with self.assertRaises(ValueError):
                list(records([text]))
        for parameters in [(-1, 1, 1), (1, 0, 1), (1, 1, 64)]:
            with self.assertRaises(ValueError):
                Cache(*parameters)

    def test_reproducible_random_input(self):
        text = random_trace(3, 4, 5, 73)
        self.assertEqual(text, random_trace(3, 4, 5, 73))
        parsed = list(records(text.splitlines()))
        self.assertEqual(len(parsed), 300)
        self.assertTrue(any(address > 0xffffffff for _, address, _ in parsed))


FIXTURE = r'''
#include "cachelab.h"
#include <stddef.h>
static void baseline(int M, int N, int A[N][M], int B[M][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            B[j][i] = A[i][j];
}
static void empty(int M, int N, int A[N][M], int B[M][N]) {
    (void)M; (void)N; (void)A; (void)B;
}
static void corrupt_a(int M, int N, int A[N][M], int B[M][N]) {
    baseline(M, N, A, B);
    A[0][0] ^= 1;
}
static void out_of_bounds(int M, int N, int A[N][M], int B[M][N]) {
    baseline(M, N, A, B);
    volatile int *p = &B[0][0];
    p[(size_t)M * N] = 1;
}
void registerFunctions(void) {
    registerTransFunction(baseline, "Transpose submission");
    registerTransFunction(empty, "empty");
    registerTransFunction(corrupt_a, "corrupt A");
    registerTransFunction(out_of_bounds, "out of bounds");
}
'''


class NativeTracingChecks(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temporary = tempfile.TemporaryDirectory(prefix="cachelab-tools-check-")
        cls.addClassCleanup(cls.temporary.cleanup)
        cls.directory = Path(cls.temporary.name)
        source = cls.directory / "fixture.c"
        source.write_text(FIXTURE)
        cls.executable = build_trans(cls.directory, source, "clang")
        cls.native_cli = cls.directory / "test-trans"
        subprocess.run([
            "clang", "-std=c99", "-g", "-O0", "-Wall", "-Werror",
            "-fsanitize=address", "-DNATIVE_TRANS_LIBRARY", "-I", str(ROOT),
            str(ROOT / "tools/native_test_trans.c"),
            str(ROOT / "tools/native_trans.c"), str(ROOT / "cachelab.c"),
            str(cls.directory / "trans.o"), "-o", str(cls.native_cli),
        ], check=True, timeout=60)

    def run_fixture(self, fn, M=7, N=9):
        trace = self.directory / "fixture.trace"
        result = run_trans(self.executable, M, N, fn, 7, trace, 262144, self.directory)
        return result, trace

    def test_exact_access_order(self):
        for M, N in [(1, 1), (7, 9), (32, 32), (61, 67)]:
            with self.subTest(size=(M, N)):
                (ok, detail, counts), trace = self.run_fixture(0, M, N)
                self.assertTrue(ok, detail)
                self.assertEqual(sum(counts[:2]), 2 * M * N)
                expected = []
                for i in range(N):
                    for j in range(M):
                        expected += [("L", (i * M + j) * 4, 4),
                                     ("S", 262144 + (j * N + i) * 4, 4)]
                with trace.open() as stream:
                    self.assertEqual(list(records(stream)), expected)
                if M == N == 32:
                    self.assertEqual(counts, (868, 1180, 1148))

    def test_wrong_result_is_rejected(self):
        (ok, detail, _), _ = self.run_fixture(1)
        self.assertFalse(ok)
        self.assertIn("Wrong B[", detail)

    def test_input_modification_is_rejected(self):
        (ok, detail, _), _ = self.run_fixture(2)
        self.assertFalse(ok)
        self.assertIn("Input matrix A was modified", detail)

    def test_out_of_bounds_is_rejected(self):
        (ok, detail, _), _ = self.run_fixture(3)
        self.assertFalse(ok)
        self.assertIn("AddressSanitizer", detail)

    def test_invalid_function_is_rejected(self):
        (ok, detail, _), _ = self.run_fixture(100)
        self.assertFalse(ok)
        self.assertIn("not registered", detail)

    def run_cli(self, *arguments):
        return subprocess.run([str(self.native_cli), *map(str, arguments)],
                              cwd=self.directory, capture_output=True,
                              text=True, timeout=20)

    def test_native_cli_cache_counts_match_trace_oracle(self):
        for M, N in [(1, 1), (7, 9), (32, 32), (64, 64), (61, 67)]:
            with self.subTest(size=(M, N)):
                (ok, detail, expected), _ = self.run_fixture(0, M, N)
                self.assertTrue(ok, detail)
                result = self.run_cli('-M', M, '-N', N, '-F', 0)
                self.assertEqual(result.returncode, 0, result.stderr)
                match = re.search(r'hits:(\d+) misses:(\d+) evictions:(\d+)', result.stdout)
                self.assertIsNotNone(match, result.stdout)
                self.assertEqual(tuple(map(int, match.groups())), expected)
                self.assertIn(f'TEST_TRANS_RESULTS=1:{expected[1]}', result.stdout)

    def test_native_cli_detects_wrong_output_and_input_changes(self):
        for fn, message in [(1, 'Wrong B['), (2, 'Input matrix A was modified')]:
            with self.subTest(function=fn):
                result = self.run_cli('-M', 7, '-N', 9, '-F', fn)
                self.assertEqual(result.returncode, 1, result.stderr)
                self.assertIn(message, result.stderr)
                self.assertIn('TEST_TRANS_RESULTS=0:0', result.stdout)

    def test_native_cli_survives_out_of_bounds_function(self):
        result = self.run_cli('-M', 7, '-N', 9, '-F', 3)
        self.assertEqual(result.returncode, 1, result.stderr)
        self.assertIn('AddressSanitizer', result.stderr)
        self.assertIn('TEST_TRANS_RESULTS=0:0', result.stdout)

    def test_native_cli_reports_submission_despite_other_function_failures(self):
        result = self.run_cli('-M', 7, '-N', 9)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('PASS func 0 (Transpose submission)', result.stdout)
        self.assertIn('FAIL func 1', result.stdout)
        self.assertIn('FAIL func 2', result.stdout)
        self.assertIn('FAIL func 3', result.stdout)
        self.assertIn('TEST_TRANS_RESULTS=1:37', result.stdout)

    def test_native_cli_validates_arguments(self):
        for arguments in [[], ['-M', 32], ['-M', 0, '-N', 32],
                          ['-M', 32, '-N', 257], ['-M', '32oops', '-N', 32],
                          ['-M', 32, '-N', 32, '-F', -1],
                          ['-M', 32, '-N', 32, '-F', 100]]:
            with self.subTest(arguments=arguments):
                self.assertEqual(self.run_cli(*arguments).returncode, 2)


if __name__ == "__main__":
    unittest.main()
