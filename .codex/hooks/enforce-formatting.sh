#!/bin/bash

set -u

Fail() {
    printf '%s\n' "$1" >&2
    exit 2
}

if [[ $# -ne 1 ]]; then
    Fail 'Memly formatting integrity requires exactly one mode: --write or --check.'
fi

case "$1" in
    --write)
        ClangFormatArgumentArray=( -i )
        CMakeFormatArgumentArray=( -i )
        PrettierArgument=--write
        ;;
    --check)
        ClangFormatArgumentArray=( --dry-run --Werror )
        CMakeFormatArgumentArray=( --check )
        PrettierArgument=--check
        ;;
    *)
        Fail 'Memly formatting integrity received an unsupported mode.'
        ;;
esac

RequiredPrettierVersion=3.9.6
PrettierExecutable="$(command -v prettier)" || Fail "Memly formatting requires Prettier $RequiredPrettierVersion on PATH."
PrettierVersion="$("$PrettierExecutable" --version)" || Fail 'Memly formatting could not read the installed Prettier version.'
if [[ "$PrettierVersion" != "$RequiredPrettierVersion" ]]; then
    Fail "Memly formatting requires Prettier $RequiredPrettierVersion; found $PrettierVersion at $PrettierExecutable."
fi

if ! find program -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \) -exec clang-format "${ClangFormatArgumentArray[@]}" {} +; then
    Fail 'Memly C++ formatting failed.'
fi

if ! cmake-format "${CMakeFormatArgumentArray[@]}" CMakeLists.txt; then
    Fail 'Memly CMake formatting failed.'
fi

MarkdownFileList="$(git ls-files -co --exclude-standard -- '*.md' '*.markdown')" || Fail 'Memly Markdown file discovery failed.'
MarkdownFilePathArray=()
while IFS= read -r FilePath; do
    if [[ -n "$FilePath" && -f "$FilePath" ]]; then
        MarkdownFilePathArray+=( "$FilePath" )
    fi
done <<< "$MarkdownFileList"

if [[ ${#MarkdownFilePathArray[@]} -gt 0 ]] && ! "$PrettierExecutable" "$PrettierArgument" "${MarkdownFilePathArray[@]}"; then
    Fail 'Memly Markdown formatting failed.'
fi

YamlFileList="$(git ls-files -co --exclude-standard -- '*.yaml' '*.yml' '.clang-tidy')" || Fail 'Memly YAML file discovery failed.'
YamlFilePathArray=()
while IFS= read -r FilePath; do
    if [[ -n "$FilePath" && -f "$FilePath" ]]; then
        YamlFilePathArray+=( "$FilePath" )
    fi
done <<< "$YamlFileList"

if [[ ${#YamlFilePathArray[@]} -gt 0 ]] && ! "$PrettierExecutable" "$PrettierArgument" "${YamlFilePathArray[@]}"; then
    Fail 'Memly YAML formatting failed.'
fi
