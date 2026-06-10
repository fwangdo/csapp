# Official CS:APP Labs

Source: <https://csapp.cs.cmu.edu/3e/labs.html>

The official page says the CS:APP3e labs are distributed as self-contained tar
files. Self-study handouts do not include solutions. Downloaded tarballs are kept
in ignored `labs/_handouts/`; working copies are already unpacked into
`labs/<lab-name>/`.

| Lab | Updated on official page | Local handout | Chapter fit |
| --- | --- | --- | --- |
| Data Lab | 2019-12-16 | `datalab-handout.tar` | Chapter 2 |
| Bomb Lab | 2016-01-12 | `bomb.tar` | Chapter 3 |
| Attack Lab | 2016-01-11 | `target1.tar` | Chapter 3 |
| Architecture Lab | 2016-10-19 | `archlab-handout.tar` | Chapter 4 |
| Cache Lab | 2016-05-02 | `cachelab-handout.tar` | Chapter 6 |
| Performance Lab | 2014-09-02 | `perflab-handout.tar` | Chapter 5 |
| Shell Lab | 2003-07-28 | `shlab-handout.tar` | Chapter 8 |
| Malloc Lab | 2014-09-02 | `malloclab-handout.tar` | Chapter 9 |
| Proxy Lab | 2019-11-14 | `proxylab-handout.tar` | Chapters 10-12 |

Legacy note: Buffer Lab is listed on the official page, but CS:APP3e marks it as
the old IA32 lab replaced by Attack Lab. Do not start with it unless there is a
specific reason to study 32-bit exploitation separately.

## Refreshing a Lab

```sh
scripts/fetch-official-labs.sh
mkdir -p labs/datalab
tar -xvf labs/_handouts/datalab-handout.tar -C labs/datalab --strip-components=1
```

Use this only when refreshing from the official tarball. Otherwise, use the
lab's own README, writeup, Makefile, and tests as the source of truth. Add only
small local notes unless repeated work proves a helper script is needed.
