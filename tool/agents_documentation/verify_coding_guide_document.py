#!/usr/bin/env python3

import pathlib
import re
import sys


CODING_GUIDE_PATH = pathlib.Path("documentation/agents/CODING_GUIDE.md")
BUILT_IN_CLANG_TIDY_CHECK_PATTERN = re.compile(
    r"\b(?:abseil|altera|android|boost|bugprone|cert|clang-analyzer|clang-diagnostic|concurrency|cppcoreguidelines|"
    r"darwin|fuchsia|google|hicpp|linuxkernel|llvm|llvmlibc|misc|modernize|mpi|objc|openmp|performance|portability|"
    r"readability|zircon)-[a-z0-9][a-z0-9-]*\b"
)
COMPILER_WARNING_FLAG_PATTERN = re.compile(r"(?<![A-Za-z0-9_])-W[A-Za-z0-9][A-Za-z0-9_+=-]*")


def source_position(source_text: str, character_offset: int) -> tuple[int, int]:
    line_start_offset = source_text.rfind("\n", 0, character_offset) + 1
    return source_text.count("\n", 0, character_offset) + 1, character_offset - line_start_offset + 1


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: verify_coding_guide_document.py <repository-root>", file=sys.stderr)
        return 2

    repository_root_path = pathlib.Path(sys.argv[1]).resolve()
    coding_guide_path = repository_root_path / CODING_GUIDE_PATH
    source_text = coding_guide_path.read_text(encoding="utf-8")
    violation_matches = sorted(
        (
            match
            for pattern in (BUILT_IN_CLANG_TIDY_CHECK_PATTERN, COMPILER_WARNING_FLAG_PATTERN)
            for match in pattern.finditer(source_text)
        ),
        key=lambda match: match.start(),
    )
    for violation_match in violation_matches:
        line_number, column_number = source_position(source_text, violation_match.start())
        print(
            f"{CODING_GUIDE_PATH}:{line_number}:{column_number}: error: "
            f"the coding guide must not enumerate built-in diagnostic '{violation_match.group()}' "
            "[memly-coding-guide-scope]",
            file=sys.stderr,
        )

    return int(bool(violation_matches))


if __name__ == "__main__":
    raise SystemExit(main())
