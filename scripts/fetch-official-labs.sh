#!/usr/bin/env sh
set -eu

dest="labs/_handouts"
mkdir -p "$dest"

# Official CS:APP3e self-study handouts.
# Source checked from:
# https://csapp.cs.cmu.edu/3e/labs.html
base="https://csapp.cs.cmu.edu/3e"

fetch() {
  name="$1"
  url="$2"
  out="$dest/$name"

  if [ -f "$out" ]; then
    printf '%s already exists\n' "$out"
    return
  fi

  printf 'downloading %s\n' "$name"
  curl -L --fail --output "$out" "$url"
}

fetch datalab-handout.tar "$base/datalab-handout.tar"
fetch bomb.tar "$base/bomb.tar"
fetch target1.tar "$base/target1.tar"
fetch archlab-handout.tar "$base/archlab-handout.tar"
fetch cachelab-handout.tar "$base/cachelab-handout.tar"
fetch perflab-handout.tar "$base/perflab-handout.tar"
fetch shlab-handout.tar "$base/shlab-handout.tar"
fetch malloclab-handout.tar "$base/malloclab-handout.tar"
fetch proxylab-handout.tar "$base/proxylab-handout.tar"

cat <<'EOF'

Handouts are in labs/_handouts.
Example:
  mkdir -p labs/datalab
  tar -xvf labs/_handouts/datalab-handout.tar -C labs/datalab --strip-components=1

EOF
