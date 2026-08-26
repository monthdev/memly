#!/usr/bin/env python3

import pathlib
import sys


ALLOWED_SOURCE_SUFFIXES = frozenset((".cpp", ".mpp"))
C_AND_CPP_SOURCE_SUFFIXES = frozenset(
    (
        ".c",
        ".c++",
        ".cc",
        ".ccm",
        ".cp",
        ".cpp",
        ".cppm",
        ".cu",
        ".cuh",
        ".cxx",
        ".cxxm",
        ".h",
        ".hh",
        ".h++",
        ".hp",
        ".hpp",
        ".hxx",
        ".ii",
        ".inc",
        ".inl",
        ".ipp",
        ".ixx",
        ".m",
        ".mm",
        ".mpp",
        ".tcc",
        ".tpp",
        ".txx",
    )
)
MEMLY_SOURCE_DIRECTORY_NAMES = ("source", "test")


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: verify_source_extensions.py <repository-root>", file=sys.stderr)
        return 2

    repository_root_path = pathlib.Path(sys.argv[1]).resolve()
    invalid_source_paths = sorted(
        source_path.relative_to(repository_root_path)
        for source_directory_name in MEMLY_SOURCE_DIRECTORY_NAMES
        if (source_directory_path := repository_root_path / source_directory_name).is_dir()
        for source_path in source_directory_path.rglob("*")
        if source_path.is_file()
        and source_path.suffix.lower() in C_AND_CPP_SOURCE_SUFFIXES
        and source_path.suffix not in ALLOWED_SOURCE_SUFFIXES
    )
    if not invalid_source_paths:
        return 0

    print(
        "Memly-authored module interfaces must use .mpp and implementation units must use .cpp; "
        "C, Objective-C, Objective-C++, headers, and alternate C++ source extensions are disallowed:",
        file=sys.stderr,
    )
    for invalid_source_path in invalid_source_paths:
        print(f"  {invalid_source_path}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
