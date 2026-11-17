# Test traces

These 11 trace files match the filenames in `config.h` and were downloaded
on 2026-11-17 from the [prachikothari/malloc-lab trace directory](https://github.com/prachikothari/malloc-lab/tree/master/traces).
Only trace data was imported. Each downloaded file was checked against the
Git blob SHA reported by the repository contents API, and its operation
count was checked against its header.

Run `make test` from the parent directory to test correctness, space
utilization, and throughput with the provided driver.
