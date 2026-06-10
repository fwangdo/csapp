# csapp

Personal workspace for studying *Computer Systems: A Programmer's Perspective*.

The repository is intentionally small: official lab handouts stay untracked, while
my implementations, notes, and small chapter experiments stay in git.

## Layout

- `chapters/`: small chapter-by-chapter implementation starters.
- `labs/`: working copies of official CS:APP labs.
- `labs/_handouts/`: downloaded upstream tarballs, ignored by git.
- `docs/`: local PDFs and reference material, ignored by git.
- `notes/`: durable study notes.
- `scripts/`: setup helpers.

## Setup

Install the basic compiler tools first:

```sh
xcode-select --install
```

On macOS, `/usr/bin/gcc` is usually Apple clang, not GNU GCC. Install GNU GCC
with Homebrew when CS:APP material expects GCC-specific behavior:

```sh
brew install gcc
```

Homebrew installs versioned commands such as `gcc-15`, `g++-15`, and
`gcc-ar-15`. Use those names explicitly:

```sh
gcc-15 --version
gcc-15 -Wall -Wextra -std=c11 chapters/ch01-tour/hello.c -o /tmp/hello
make CC=gcc-15 -C chapters/ch07-linking
```

For one shell session, this also works:

```sh
export CC=gcc-15
make -C chapters/ch07-linking
```

Do not rely on plain `gcc` unless `gcc --version` says `gcc (Homebrew GCC ...)`.
CS:APP labs that depend on Linux/x86-64 or ELF behavior may still need a Linux
VM or container; Homebrew GCC only changes the compiler on macOS.

Then download official self-study lab handouts:

```sh
scripts/fetch-official-labs.sh
```

Unpack a lab into `labs/<lab-name>/` when starting it. Keep the original handout
close by, but commit only my work and short notes.

## Study Flow

1. Read the chapter section.
2. Run or fill in the matching starter under `chapters/`.
3. Move to the official lab when the chapter maps to one.
4. Keep temporary references in `docs/` or `tmp/`; keep durable notes in `notes/`.
