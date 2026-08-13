#!/bin/sh

set -u

RepositoryRoot="$(git rev-parse --show-toplevel)" || exit 2
cd "$RepositoryRoot" || exit 2

if ! bash .codex/hooks/enforce-formatting.sh --write >&2; then
    printf '%s\n' 'Memly formatting integrity failed. Repair the reported formatting failure before completing the patch.' >&2
    exit 2
fi

if ! cmake --preset macos-debug-local >&2; then
    printf '%s\n' 'Memly integrity configuration failed. Diagnose and repair it before completing the patch.' >&2
    exit 2
fi

if ! cmake --build --preset macos-debug-local >&2; then
    printf '%s\n' 'Memly integrity build failed. Diagnose and repair it before completing the patch.' >&2
    exit 2
fi

printf '{}\n'
