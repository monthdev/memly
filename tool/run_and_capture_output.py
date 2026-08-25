#!/usr/bin/env python3

import argparse
import hashlib
import json
import os
import platform
import shlex
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("report", type=Path)
    parser.add_argument("--source-root", required=True, type=Path)
    parser.add_argument("--config-file", required=True, type=Path)
    parser.add_argument("--compilation-database", required=True, type=Path)
    parser.add_argument("--tool", required=True, type=Path)
    parser.add_argument("--metadata", action="append", default=[])
    parser.add_argument("--input-file", action="append", required=True, type=Path)
    try:
        command_separator_index = sys.argv.index("--")
    except ValueError:
        parser.error("a command is required after --")
    arguments = parser.parse_args(sys.argv[1:command_separator_index])
    arguments.command = sys.argv[command_separator_index + 1 :]
    if not arguments.command:
        parser.error("a command is required after --")
    return arguments


def file_sha256(file_path: Path) -> str:
    file_hash = hashlib.sha256()
    with file_path.open("rb") as input_file:
        for file_bytes in iter(lambda: input_file.read(1024 * 1024), b""):
            file_hash.update(file_bytes)
    return file_hash.hexdigest()


def command_output(command: list[str], working_directory: Path) -> str:
    return subprocess.run(
        command,
        cwd=working_directory,
        check=True,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    ).stdout.strip()


def source_snapshot_sha256(input_file_paths: list[Path], source_root: Path) -> str:
    source_root = source_root.resolve()
    source_snapshot_hash = hashlib.sha256()
    for input_file_path in sorted(input_file_paths):
        resolved_input_file_path = input_file_path.resolve()
        source_snapshot_hash.update(str(resolved_input_file_path.relative_to(source_root)).encode("utf-8"))
        source_snapshot_hash.update(b"\0")
        with resolved_input_file_path.open("rb") as input_file:
            for input_file_bytes in iter(lambda: input_file.read(1024 * 1024), b""):
                source_snapshot_hash.update(input_file_bytes)
        source_snapshot_hash.update(b"\0")
    return source_snapshot_hash.hexdigest()


def profile_metadata(arguments: argparse.Namespace) -> list[str]:
    git_status = command_output(
        ["git", "status", "--porcelain=v1", "--untracked-files=all"], arguments.source_root
    )
    git_branch_process = subprocess.run(
        ["git", "symbolic-ref", "--quiet", "--short", "HEAD"],
        cwd=arguments.source_root,
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )
    git_branch = git_branch_process.stdout.strip() or "detached"
    with arguments.compilation_database.open(encoding="utf-8") as compilation_database_file:
        compilation_database_entry_count = len(json.load(compilation_database_file))

    metadata = [
        "Clang-Tidy profile metadata",
        f"Started UTC: {datetime.now(timezone.utc).isoformat(timespec='seconds')}",
        f"Host: {platform.platform()}",
        f"Logical processors: {os.cpu_count()}",
        f"Source root: {arguments.source_root.resolve()}",
        f"Git commit: {command_output(['git', 'rev-parse', 'HEAD'], arguments.source_root)}",
        f"Git branch: {git_branch}",
        f"Git worktree: {'dirty' if git_status else 'clean'}",
        f"Git changed paths: {len(git_status.splitlines()) if git_status else 0}",
        f"Lint source snapshot SHA-256: {source_snapshot_sha256(arguments.input_file, arguments.source_root)}",
        f"Lint source files: {len(arguments.input_file)}",
        f"Clang-Tidy config SHA-256: {file_sha256(arguments.config_file)}",
        f"Compilation database SHA-256: {file_sha256(arguments.compilation_database)}",
        f"Compilation database entries: {compilation_database_entry_count}",
        f"Clang-Tidy version: {command_output([str(arguments.tool), '--version'], arguments.source_root)}",
        *arguments.metadata,
        f"Command: {shlex.join(arguments.command)}",
        "",
    ]
    return metadata


def main() -> int:
    arguments = parse_arguments()

    arguments.report.parent.mkdir(parents=True, exist_ok=True)
    with arguments.report.open("w", encoding="utf-8") as report_file:
        for metadata_line in profile_metadata(arguments):
            print(metadata_line)
            report_file.write(f"{metadata_line}\n")
        report_file.flush()
        with subprocess.Popen(
            arguments.command,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            encoding="utf-8",
            errors="replace",
        ) as process:
            assert process.stdout is not None
            for output_line in process.stdout:
                sys.stdout.write(output_line)
                sys.stdout.flush()
                report_file.write(output_line)
            return_code = process.wait()

    print(f"Clang-Tidy profile report: {arguments.report}")
    return return_code


if __name__ == "__main__":
    raise SystemExit(main())
