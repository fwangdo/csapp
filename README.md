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
