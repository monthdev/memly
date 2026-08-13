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
        FormatMode=write
        ;;
    --check)
        FormatMode=check
        ;;
    *)
        Fail 'Memly formatting integrity received an unsupported mode.'
        ;;
esac

RequireExecutable() {
    local ExecutableName="$1"
    local RequiredVersion="$2"
    local VersionArgument="$3"
    local ExpectedVersionOutput="$4"

    RequiredExecutablePath="$(command -v "$ExecutableName")" || Fail "Memly formatting requires $ExecutableName $RequiredVersion on PATH."
    local InstalledVersionOutput
    InstalledVersionOutput="$("$RequiredExecutablePath" "$VersionArgument")" || Fail "Memly formatting could not read the installed $ExecutableName version."
    if [[ "$InstalledVersionOutput" != "$ExpectedVersionOutput" ]]; then
        Fail "Memly formatting requires $ExecutableName $RequiredVersion; found $InstalledVersionOutput at $RequiredExecutablePath."
    fi
}

RequireExecutableContainingVersion() {
    local ExecutableName="$1"
    local RequiredVersion="$2"
    local ExpectedVersionFragment="$3"

    RequiredExecutablePath="$(command -v "$ExecutableName")" || Fail "Memly formatting requires $ExecutableName $RequiredVersion on PATH."
    local InstalledVersionOutput
    InstalledVersionOutput="$("$RequiredExecutablePath" --version)" || Fail "Memly formatting could not read the installed $ExecutableName version."
    if [[ "$InstalledVersionOutput" != *"$ExpectedVersionFragment"* ]]; then
        Fail "Memly formatting requires $ExecutableName $RequiredVersion; found $InstalledVersionOutput at $RequiredExecutablePath."
    fi
}

CollectExistingFiles() {
    local FormatName="$1"
    shift

    local FileList
    FileList="$(git ls-files -co --exclude-standard -- "$@")" || Fail "Memly $FormatName file discovery failed."
    FilePathArray=()
    while IFS= read -r FilePath; do
        if [[ -n "$FilePath" && -f "$FilePath" ]]; then
            FilePathArray+=( "$FilePath" )
        fi
    done <<< "$FileList"
}

FormatFileArrayWithOutput() {
    local FormatName="$1"
    shift

    local FormattingDifferenceFound=false
    for FilePath in "${FilePathArray[@]}"; do
        if ! "$@" "$FilePath" > "$TemporaryFormattedFilePath"; then
            Fail "Memly $FormatName formatting failed for $FilePath."
        fi
        if cmp -s "$FilePath" "$TemporaryFormattedFilePath"; then
            continue
        fi

        if [[ "$FormatMode" == write ]]; then
            if ! cp "$TemporaryFormattedFilePath" "$FilePath"; then
                Fail "Memly $FormatName formatting could not update $FilePath."
            fi
            printf 'Formatted %s\n' "$FilePath" >&2
        else
            diff -u "$FilePath" "$TemporaryFormattedFilePath" >&2 || true
            FormattingDifferenceFound=true
        fi
    done

    if [[ "$FormattingDifferenceFound" == true ]]; then
        Fail "Memly $FormatName formatting found files requiring changes."
    fi
}

TemporaryFormattedFilePath="$(mktemp "${TMPDIR:-/tmp}/memly-formatted-file.XXXXXX")" || Fail 'Memly formatting could not create a temporary file.'
trap 'rm -f "$TemporaryFormattedFilePath"' EXIT

RequireExecutableContainingVersion clang-format 22.x 'version 22.'
ClangFormatExecutable="$RequiredExecutablePath"
CollectExistingFiles C++ '*.c' '*.cc' '*.cpp' '*.cxx' '*.h' '*.hh' '*.hpp' '*.hxx' '*.m' '*.mm'
FormatFileArrayWithOutput C++ "$ClangFormatExecutable" -style=file

RequireExecutable cmake-format 0.6.13 --version 0.6.13
CMakeFormatExecutable="$RequiredExecutablePath"
CollectExistingFiles CMake 'CMakeLists.txt' ':(glob)**/CMakeLists.txt' ':(glob)**/*.cmake' ':(glob)**/*.cmake.in'
FormatFileArrayWithOutput CMake "$CMakeFormatExecutable"

RequireExecutable prettier 3.9.6 --version 3.9.6
PrettierExecutable="$RequiredExecutablePath"
CollectExistingFiles Markdown '*.md' '*.markdown'
if [[ ${#FilePathArray[@]} -gt 0 ]]; then
    if [[ "$FormatMode" == write ]]; then
        "$PrettierExecutable" --write --log-level warn "${FilePathArray[@]}" || Fail 'Memly Markdown formatting failed.'
    else
        "$PrettierExecutable" --check "${FilePathArray[@]}" || Fail 'Memly Markdown formatting failed.'
    fi
fi

RequireExecutable qmlformat 6.11.1 --version 'qmlformat 6.11.1'
QmlFormatExecutable="$RequiredExecutablePath"
CollectExistingFiles QML '*.qml'
FormatFileArrayWithOutput QML "$QmlFormatExecutable" --settings .qmlformat.ini

RequireExecutable sql-formatter 15.8.2 --version 15.8.2
SqlFormatterExecutable="$RequiredExecutablePath"
CollectExistingFiles SQL '*.sql'
FormatFileArrayWithOutput SQL "$SqlFormatterExecutable" --language duckdb

CollectExistingFiles YAML '*.yaml' '*.yml' '.clang-tidy'
if [[ ${#FilePathArray[@]} -gt 0 ]]; then
    if [[ "$FormatMode" == write ]]; then
        "$PrettierExecutable" --write --log-level warn "${FilePathArray[@]}" || Fail 'Memly YAML formatting failed.'
    else
        "$PrettierExecutable" --check "${FilePathArray[@]}" || Fail 'Memly YAML formatting failed.'
    fi
fi
