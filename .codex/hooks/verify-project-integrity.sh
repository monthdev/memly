#!/usr/bin/env bash

set -u

RepositoryRoot="$(git rev-parse --show-toplevel)" || exit 2
cd "$RepositoryRoot" || exit 2

if ! bash .codex/hooks/enforce-formatting.sh --write >&2; then
    printf '%s\n' 'Memly formatting integrity failed. Repair the reported formatting failure before completing the patch.' >&2
    exit 2
fi

BuildDirectory='build/macos-debug'
PresetFingerprintFilePath="$BuildDirectory/.memly-integrity-preset-fingerprint"
PresetFingerprintInput=''
for PresetFilePath in CMakePresets.json CMakeUserPresets.json; do
    if [ -f "$PresetFilePath" ]; then
        PresetFileHash="$(git hash-object "$PresetFilePath")" || {
            printf '%s\n' "Memly integrity configuration could not fingerprint $PresetFilePath." >&2
            exit 2
        }
    else
        PresetFileHash=missing
    fi
    PresetFingerprintInput="${PresetFingerprintInput}
${PresetFilePath}
${PresetFileHash}"
done
PresetFingerprint="$(printf '%s\n' "$PresetFingerprintInput" | git hash-object --stdin)" || {
    printf '%s\n' 'Memly integrity configuration could not fingerprint the selected preset files.' >&2
    exit 2
}

ConfiguredPresetFingerprint=''
if [ -f "$PresetFingerprintFilePath" ]; then
    ConfiguredPresetFingerprint="$(sed -n '1p' "$PresetFingerprintFilePath")"
fi

if [ ! -f "$BuildDirectory/build.ninja" ] || [ "$PresetFingerprint" != "$ConfiguredPresetFingerprint" ]; then
    if ! cmake --preset macos-debug-local >&2; then
        printf '%s\n' 'Memly integrity configuration failed. Diagnose and repair it before completing the patch.' >&2
        exit 2
    fi
    if ! printf '%s\n' "$PresetFingerprint" > "$PresetFingerprintFilePath"; then
        printf '%s\n' 'Memly integrity configuration could not record its preset fingerprint.' >&2
        exit 2
    fi
fi

if ! cmake --build --preset macos-debug-local >&2; then
    printf '%s\n' 'Memly integrity build or mandatory lint analysis failed. Diagnose and repair it before completing the patch.' >&2
    exit 2
fi

printf '{}\n'
