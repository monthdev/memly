#!/usr/bin/env python3

import argparse
import json
import os
import pathlib
import re
import shutil
import subprocess
import sys


HEADER_SUFFIXES = (".hpp",)
TRANSLATION_UNIT_SUFFIXES = (".cpp",)
DIAGNOSTIC_PATTERN = re.compile(r"^(.*?):[0-9]+:[0-9]+:\s")
INCLUDE_DIAGNOSTIC_PATTERN = re.compile(r"^.*?:[0-9]+:[0-9]+:\s+(?:error|warning):\s+(?:add|superfluous)\s")


def parse_arguments() -> argparse.Namespace:
    argument_parser = argparse.ArgumentParser()
    argument_parser.add_argument("compilation_database")
    argument_parser.add_argument("mapping_file")
    argument_parser.add_argument("selected_header", nargs="?")
    argument_parser.add_argument("--header", action="append", default=[])
    argument_parser.add_argument("--translation-unit", action="append", default=[])
    argument_parser.add_argument("--output-format", choices=("clang", "clang-warning"), default="clang-warning")
    argument_parser.add_argument("--include-what-you-use-driver")
    argument_parser.add_argument("--include-what-you-use-executable")
    argument_parser.add_argument("--project-root")
    return argument_parser.parse_args()


def resolve_compile_command_file(compile_command: dict[str, object]) -> pathlib.Path | None:
    compile_command_file = compile_command.get("file")
    if not isinstance(compile_command_file, str):
        return None
    compile_command_path = pathlib.Path(compile_command_file)
    if compile_command_path.is_absolute():
        return compile_command_path.resolve()
    compile_command_directory = compile_command.get("directory")
    if not isinstance(compile_command_directory, str):
        return None
    return pathlib.Path(compile_command_directory, compile_command_path).resolve()


def find_header_probe(compilation_database: list[dict[str, object]], header_path: pathlib.Path) -> pathlib.Path | None:
    relative_header_paths: list[pathlib.Path] = []
    for path_index, path_part in enumerate(header_path.parts):
        if path_part in ("include", "source"):
            relative_header_paths.append(pathlib.Path(*header_path.parts[path_index + 1 :]))

    header_probe_paths: list[pathlib.Path] = []
    for compile_command in compilation_database:
        header_probe_path = resolve_compile_command_file(compile_command)
        if header_probe_path is None:
            continue
        if any(
            header_probe_path.as_posix().endswith(f"/{relative_header_path.as_posix()}.cxx")
            for relative_header_path in relative_header_paths
        ):
            header_probe_paths.append(header_probe_path)

    if len(header_probe_paths) != 1:
        return None
    return header_probe_paths[0]


def get_component_root(source_path: pathlib.Path) -> pathlib.Path | None:
    for parent_path in source_path.parents:
        if parent_path.name in ("include", "source"):
            return parent_path.parent
    return None


def find_associated_translation_unit(
    header_path: pathlib.Path, translation_unit_paths: list[pathlib.Path]
) -> pathlib.Path | None:
    header_component_root = get_component_root(header_path)
    associated_translation_unit_paths = [
        translation_unit_path
        for translation_unit_path in translation_unit_paths
        if translation_unit_path.stem == header_path.stem
        and get_component_root(translation_unit_path) == header_component_root
    ]
    if len(associated_translation_unit_paths) != 1:
        return None
    return associated_translation_unit_paths[0]


def write_nonempty_lines(output: str, output_stream: object) -> None:
    for output_line in output.splitlines():
        if output_line.strip():
            print(output_line, file=output_stream)


def run_include_what_you_use(
    compilation_database_path: pathlib.Path,
    mapping_file_path: pathlib.Path,
    source_paths: list[pathlib.Path],
    output_format: str,
    include_what_you_use_driver_path: pathlib.Path,
    include_what_you_use_executable_path: pathlib.Path,
) -> subprocess.CompletedProcess[str] | None:
    if not source_paths:
        return None

    include_what_you_use_environment = os.environ.copy()
    include_what_you_use_environment["PATH"] = (
        f"{include_what_you_use_executable_path.parent}{os.pathsep}"
        f"{include_what_you_use_environment.get('PATH', '')}"
    )
    return subprocess.run(
        [
            sys.executable,
            str(include_what_you_use_driver_path),
            "-o",
            output_format,
            "-j",
            "-p",
            str(compilation_database_path),
            *(str(source_path) for source_path in source_paths),
            "--",
            "-Xiwyu",
            f"--mapping_file={mapping_file_path}",
            "-Xiwyu",
            "--cxx17ns",
        ],
        capture_output=True,
        check=False,
        env=include_what_you_use_environment,
        text=True,
    )


def collect_header_diagnostics(
    include_what_you_use_result: subprocess.CompletedProcess[str] | None,
    reported_header_paths: set[pathlib.Path],
) -> tuple[list[str], bool]:
    if include_what_you_use_result is None:
        return [], False

    combined_output = "\n".join(
        output for output in (include_what_you_use_result.stdout, include_what_you_use_result.stderr) if output
    )
    if include_what_you_use_result.returncode != 0:
        filtered_failure_output_lines: list[str] = []
        for output_line in combined_output.splitlines():
            diagnostic_match = DIAGNOSTIC_PATTERN.match(output_line)
            if diagnostic_match is not None and INCLUDE_DIAGNOSTIC_PATTERN.match(output_line):
                diagnostic_path = pathlib.Path(diagnostic_match.group(1)).resolve()
                if diagnostic_path not in reported_header_paths:
                    continue
            filtered_failure_output_lines.append(output_line)
        write_nonempty_lines("\n".join(filtered_failure_output_lines), sys.stderr)
        return [], True

    header_diagnostic_vector: list[str] = []
    for output_line in combined_output.splitlines():
        diagnostic_match = DIAGNOSTIC_PATTERN.match(output_line)
        if diagnostic_match is None:
            continue
        diagnostic_path = pathlib.Path(diagnostic_match.group(1)).resolve()
        if diagnostic_path in reported_header_paths:
            header_diagnostic_vector.append(output_line)
    return header_diagnostic_vector, False


def main() -> int:
    arguments = parse_arguments()
    compilation_database_path = pathlib.Path(arguments.compilation_database).resolve()
    mapping_file_path = pathlib.Path(arguments.mapping_file).resolve()

    include_what_you_use_driver = arguments.include_what_you_use_driver or shutil.which("iwyu_tool.py")
    include_what_you_use_executable = arguments.include_what_you_use_executable or shutil.which(
        "include-what-you-use"
    )
    if include_what_you_use_driver is None or include_what_you_use_executable is None:
        print("Include What You Use executables were not found", file=sys.stderr)
        return 2
    include_what_you_use_driver_path = pathlib.Path(include_what_you_use_driver).resolve()
    include_what_you_use_executable_path = pathlib.Path(include_what_you_use_executable).resolve()

    try:
        with compilation_database_path.open(encoding="utf-8") as compilation_database_file:
            compilation_database = json.load(compilation_database_file)
    except (OSError, json.JSONDecodeError) as error:
        print(f"Compilation database could not be read: {error}", file=sys.stderr)
        return 2

    compilation_database_translation_unit_paths = [
        compile_command_path
        for compile_command in compilation_database
        if (compile_command_path := resolve_compile_command_file(compile_command)) is not None
        and compile_command_path.suffix in TRANSLATION_UNIT_SUFFIXES
        and not compile_command_path.name.endswith(HEADER_SUFFIXES)
    ]

    if arguments.selected_header is not None:
        if arguments.header or arguments.translation_unit or arguments.project_root is not None:
            return 2
        header_paths = [pathlib.Path(arguments.selected_header).resolve()]
        associated_translation_unit_path = find_associated_translation_unit(
            header_paths[0], compilation_database_translation_unit_paths
        )
        translation_unit_paths = (
            [associated_translation_unit_path] if associated_translation_unit_path is not None else []
        )
    elif arguments.project_root is not None:
        project_root_path = pathlib.Path(arguments.project_root).resolve()
        project_source_root_paths = [project_root_path / "program", project_root_path / "test"]
        header_paths = sorted(
            source_path.resolve()
            for project_source_root_path in project_source_root_paths
            if project_source_root_path.exists()
            for source_path in project_source_root_path.rglob("*")
            if source_path.suffix in HEADER_SUFFIXES
        )
        translation_unit_paths = sorted(
            compile_command_path
            for compile_command_path in compilation_database_translation_unit_paths
            if any(
                compile_command_path.is_relative_to(project_source_root_path)
                for project_source_root_path in project_source_root_paths
            )
        )
    else:
        header_paths = [pathlib.Path(header_path).resolve() for header_path in arguments.header]
        translation_unit_paths = [
            pathlib.Path(translation_unit_path).resolve()
            for translation_unit_path in arguments.translation_unit
        ]
        if not header_paths or not translation_unit_paths:
            return 2

    associated_header_paths = {
        header_path
        for header_path in header_paths
        if find_associated_translation_unit(header_path, translation_unit_paths) is not None
    }
    header_probe_paths: list[pathlib.Path] = []
    header_probe_header_paths: set[pathlib.Path] = set()
    for header_path in header_paths:
        if header_path in associated_header_paths:
            continue
        header_probe_path = find_header_probe(compilation_database, header_path)
        if header_probe_path is None:
            print(f"{header_path}:1:1: error: CMake header probe was not found; configure the project first")
            return 2
        header_probe_paths.append(header_probe_path)
        header_probe_header_paths.add(header_path)

    source_result = run_include_what_you_use(
        compilation_database_path,
        mapping_file_path,
        translation_unit_paths,
        arguments.output_format,
        include_what_you_use_driver_path,
        include_what_you_use_executable_path,
    )
    header_probe_result = run_include_what_you_use(
        compilation_database_path,
        mapping_file_path,
        header_probe_paths,
        arguments.output_format,
        include_what_you_use_driver_path,
        include_what_you_use_executable_path,
    )

    source_header_diagnostics, source_failed = collect_header_diagnostics(
        source_result, associated_header_paths
    )
    header_probe_diagnostics, header_probe_failed = collect_header_diagnostics(
        header_probe_result, header_probe_header_paths
    )
    if source_failed or header_probe_failed:
        return 2

    header_diagnostics = list(dict.fromkeys(source_header_diagnostics + header_probe_diagnostics))
    write_nonempty_lines("\n".join(header_diagnostics), sys.stdout)
    return 1 if header_diagnostics else 0


if __name__ == "__main__":
    sys.exit(main())
