#!/usr/bin/env python3
"""Independent Cache Lab oracle; deliberately uses Python LRU lists, not C code."""

import argparse
import re
import sys


class Cache:
    def __init__(self, s, E, b):
        if s < 0 or b < 0 or s + b > 64 or E < 1:
            raise ValueError("require s,b >= 0, s+b <= 64 and E >= 1")
        self.s, self.E, self.b = s, E, b
        self.sets = {}  # Only occupied sets; entries are ordered oldest first.
        self.hits = self.misses = self.evictions = 0

    @property
    def counts(self):
        return self.hits, self.misses, self.evictions

    def access(self, address):
        block = address >> self.b
        index = block & ((1 << self.s) - 1)
        tag = block >> self.s
        lines = self.sets.setdefault(index, [])
        if tag in lines:
            self.hits += 1
            lines.remove(tag)
            event = "hit"
        else:
            self.misses += 1
            event = "miss"
            if len(lines) == self.E:
                lines.pop(0)
                self.evictions += 1
                event += " eviction"
        lines.append(tag)
        return event

    def operation(self, op, address):
        if op == "I":
            return []
        if op not in "LSM":
            raise ValueError(f"unknown operation: {op!r}")
        return [self.access(address) for _ in range(2 if op == "M" else 1)]


def records(lines):
    for number, line in enumerate(lines, 1):
        if not line.strip():
            continue
        match = re.fullmatch(r"\s*([ILSM])\s+([0-9a-fA-F]+),\s*(\d+)\s*", line)
        if not match:
            raise ValueError(f"trace line {number}: invalid record {line.strip()!r}")
        op, address, size = match.groups()
        address, size = int(address, 16), int(size)
        if address >= 1 << 64 or size < 1:
            raise ValueError(f"trace line {number}: invalid address or size")
        yield op, address, size


def simulate(lines, s, E, b, verbose=None):
    cache = Cache(s, E, b)
    for op, address, size in records(lines):
        # Cache Lab assumes accesses never cross block boundaries. Like csim-ref,
        # request sizes are ignored; this is not a general multi-block simulator.
        events = cache.operation(op, address)
        if verbose is not None and events:
            print(f"{op} {address:x},{size} {' '.join(events)}", file=verbose)
    return cache.counts


def summary(counts):
    return "hits:%d misses:%d evictions:%d" % counts


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-s", type=int, required=True)
    parser.add_argument("-E", type=int, required=True)
    parser.add_argument("-b", type=int, required=True)
    parser.add_argument("-t", type=argparse.FileType("r"), required=True)
    parser.add_argument("-v", action="store_true")
    args = parser.parse_args()
    try:
        with args.t as trace:
            print(summary(simulate(trace, args.s, args.E, args.b,
                                   sys.stdout if args.v else None)))
    except ValueError as error:
        parser.error(str(error))


if __name__ == "__main__":
    main()
