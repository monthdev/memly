#!/usr/bin/env bash

set -u

Fail() {
    printf '%s\n' "$1" >&2
    exit 2
}

RepositoryRootPath="$(git rev-parse --show-toplevel)" || Fail 'Memly agents-documentation verification could not locate the repository root.'
PythonExecutable="$(command -v python3)" || Fail 'Memly agents-documentation verification requires python3 on PATH.'

"$PythonExecutable" "$RepositoryRootPath/tool/agents_hook/verify_agent_coding_guide.py" "$RepositoryRootPath" ||
    Fail 'Memly agents-documentation verification failed.'
