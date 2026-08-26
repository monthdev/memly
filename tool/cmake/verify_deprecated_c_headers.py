#!/usr/bin/env python3

import pathlib
import re
import sys


CPP_SOURCE_SUFFIXES = frozenset((".cpp", ".mpp"))
DEPRECATED_C_HEADER_REPLACEMENTS = {
    "assert.h": "cassert",
    "complex.h": "complex",
    "ctype.h": "cctype",
    "errno.h": "cerrno",
    "fenv.h": "cfenv",
    "float.h": "cfloat",
    "inttypes.h": "cinttypes",
    "limits.h": "climits",
    "locale.h": "clocale",
    "math.h": "cmath",
    "setjmp.h": "csetjmp",
    "signal.h": "csignal",
    "stdarg.h": "cstdarg",
    "stddef.h": "cstddef",
    "stdint.h": "cstdint",
    "stdio.h": "cstdio",
    "stdlib.h": "cstdlib",
    "string.h": "cstring",
    "tgmath.h": "ctgmath",
    "time.h": "ctime",
    "uchar.h": "cuchar",
    "wchar.h": "cwchar",
    "wctype.h": "cwctype",
}
REDUNDANT_C_HEADERS = frozenset(("iso646.h", "stdalign.h", "stdbool.h"))
MEMLY_SOURCE_DIRECTORY_NAMES = ("source", "test")
PREPROCESSOR_DIRECTIVE_START_PATTERN = re.compile(r"^[ \t]*#")
INCLUDE_DIRECTIVE_PATTERN = re.compile(r"^[ \t]*#[ \t]*include\b(?P<operand>.*)$")
HEADER_LITERAL_PATTERN = re.compile(
    r'^[ \t]*(?:<(?P<angle_header>[^>\r\n]+)>|"(?P<quoted_header>[^"\r\n]+)")[ \t]*$'
)
RAW_STRING_START_PATTERN = re.compile(r'(?:u8|u|U|L)?R"([^ ()\\\t\v\f\r\n]{0,16})\(')


def authored_source_paths(repository_root_path: pathlib.Path) -> list[pathlib.Path]:
    return sorted(
        source_path
        for source_directory_name in MEMLY_SOURCE_DIRECTORY_NAMES
        if (source_directory_path := repository_root_path / source_directory_name).is_dir()
        for source_path in source_directory_path.rglob("*")
        if source_path.is_file() and source_path.suffix in CPP_SOURCE_SUFFIXES
    )


def requested_source_paths(
    repository_root_path: pathlib.Path, source_path_arguments: list[str]
) -> list[pathlib.Path]:
    if not source_path_arguments:
        return authored_source_paths(repository_root_path)
    return [pathlib.Path(source_path_argument).resolve() for source_path_argument in source_path_arguments]


def mask_characters(source_character_list: list[str], start_index: int, end_index: int) -> None:
    for character_index in range(start_index, end_index):
        if source_character_list[character_index] not in ("\r", "\n"):
            source_character_list[character_index] = " "


def comment_stripped_source_lines(source_text: str):
    lexical_state = "normal"
    raw_string_terminator = ""

    for line_number, source_line in enumerate(source_text.splitlines(keepends=True), start=1):
        line_starts_in_normal_state = lexical_state == "normal"
        source_character_list = list(source_line)
        character_index = 0
        while character_index < len(source_line):
            if lexical_state == "block_comment":
                block_comment_end_index = source_line.find("*/", character_index)
                if block_comment_end_index < 0:
                    mask_characters(source_character_list, character_index, len(source_line))
                    break
                mask_characters(source_character_list, character_index, block_comment_end_index + 2)
                lexical_state = "normal"
                character_index = block_comment_end_index + 2
                continue

            if lexical_state == "raw_string":
                raw_string_end_index = source_line.find(raw_string_terminator, character_index)
                if raw_string_end_index < 0:
                    mask_characters(source_character_list, character_index, len(source_line))
                    break
                mask_characters(
                    source_character_list,
                    character_index,
                    raw_string_end_index + len(raw_string_terminator),
                )
                lexical_state = "normal"
                character_index = raw_string_end_index + len(raw_string_terminator)
                continue

            if lexical_state in ('"', "'"):
                if source_line[character_index] == "\\":
                    character_index += 2
                    continue
                if source_line[character_index] == lexical_state:
                    lexical_state = "normal"
                character_index += 1
                continue

            if source_line.startswith("//", character_index):
                mask_characters(source_character_list, character_index, len(source_line))
                break
            if source_line.startswith("/*", character_index):
                lexical_state = "block_comment"
                mask_characters(source_character_list, character_index, character_index + 2)
                character_index += 2
                continue
            if raw_string_match := RAW_STRING_START_PATTERN.match(source_line, character_index):
                lexical_state = "raw_string"
                raw_string_terminator = f'){raw_string_match.group(1)}"'
                mask_characters(source_character_list, character_index, raw_string_match.end())
                character_index = raw_string_match.end()
                continue
            if source_line[character_index] in ('"', "'"):
                lexical_state = source_line[character_index]
            character_index += 1

        if lexical_state in ('"', "'") and not source_line.rstrip("\r\n").endswith("\\"):
            lexical_state = "normal"

        yield line_number, "".join(source_character_list), line_starts_in_normal_state


def include_directives(source_text: str):
    logical_line_segment_list = []
    logical_line_number = 0
    logical_line_was_spliced = False

    for line_number, source_line, line_starts_in_normal_state in comment_stripped_source_lines(source_text):
        if not logical_line_segment_list:
            if not line_starts_in_normal_state or not PREPROCESSOR_DIRECTIVE_START_PATTERN.match(source_line):
                continue
            logical_line_number = line_number

        source_line_without_ending = source_line.rstrip("\r\n")
        if source_line_without_ending.endswith("\\"):
            logical_line_segment_list.append(source_line_without_ending[:-1])
            logical_line_was_spliced = True
            continue

        logical_line_segment_list.append(source_line_without_ending)
        logical_source_line = "".join(logical_line_segment_list)
        logical_line_segment_list.clear()

        if not (include_match := INCLUDE_DIRECTIVE_PATTERN.match(logical_source_line)):
            logical_line_was_spliced = False
            continue

        header_literal_match = HEADER_LITERAL_PATTERN.fullmatch(include_match.group("operand"))
        if logical_line_was_spliced or header_literal_match is None:
            yield logical_line_number, include_match.start("operand") + 1, None
        else:
            header_group_name = "angle_header" if header_literal_match.group("angle_header") else "quoted_header"
            yield (
                logical_line_number,
                include_match.start("operand") + header_literal_match.start(header_group_name) + 1,
                header_literal_match.group(header_group_name),
            )
        logical_line_was_spliced = False


def main() -> int:
    if len(sys.argv) < 2:
        print(
            "usage: verify_deprecated_c_headers.py <repository-root> [source-file ...]",
            file=sys.stderr,
        )
        return 2

    repository_root_path = pathlib.Path(sys.argv[1]).resolve()
    source_paths = requested_source_paths(repository_root_path, sys.argv[2:])
    violation_found = False
    for source_path in source_paths:
        source_text = source_path.read_text(encoding="utf-8")
        for line_number, header_column, header_name in include_directives(source_text):
            if header_name is None:
                violation_found = True
                print(
                    f"{source_path.relative_to(repository_root_path)}:{line_number}:{header_column}: error: "
                    "include directives must name a header literal on one physical line "
                    "[memly-direct-header-include]",
                    file=sys.stderr,
                )
            elif replacement_header_name := DEPRECATED_C_HEADER_REPLACEMENTS.get(header_name):
                violation_found = True
                print(
                    f"{source_path.relative_to(repository_root_path)}:{line_number}:{header_column}: error: "
                    f"deprecated C compatibility header <{header_name}>; include <{replacement_header_name}> instead "
                    "[memly-deprecated-c-header]",
                    file=sys.stderr,
                )
            elif header_name in REDUNDANT_C_HEADERS:
                violation_found = True
                print(
                    f"{source_path.relative_to(repository_root_path)}:{line_number}:{header_column}: error: "
                    f"C compatibility header <{header_name}> has no effect in C++ and must be removed "
                    "[memly-deprecated-c-header]",
                    file=sys.stderr,
                )

    return int(violation_found)


if __name__ == "__main__":
    raise SystemExit(main())
