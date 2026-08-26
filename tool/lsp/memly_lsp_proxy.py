#!/usr/bin/env python3

"""Serve Memly's clangd LSP with custom Clang-Tidy diagnostics."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import queue
import re
import subprocess
import sys
import threading
import time
from typing import Any, BinaryIO
from urllib.parse import urlparse
from urllib.request import url2pathname


_CMAKE_CLANGD_VARIABLE = "MEMLY_LLVM_CLANGD_EXECUTABLE"
_CMAKE_CLANG_TIDY_VARIABLE = "MEMLY_LLVM_CLANG_TIDY_EXECUTABLE"
_CXX_SOURCE_SUFFIXES = frozenset((".cpp", ".cppm"))
_DIAGNOSTIC_PATTERN = re.compile(
    r"^(.*?):(\d+):(\d+):\s+(warning|error|fatal error|note):\s+(.*?)(?:\s+\[([^\]]+)\])?\s*$"
)


def _path_from_uri(uri: str) -> Path | None:
    parsed_uri = urlparse(uri)
    if parsed_uri.scheme != "file":
        return None
    uri_path = url2pathname(parsed_uri.path)
    if parsed_uri.netloc:
        uri_path = f"//{parsed_uri.netloc}{uri_path}"
    return Path(uri_path).resolve()


def _read_lsp_message(input_stream: BinaryIO) -> tuple[bytes, dict[str, Any]] | None:
    content_length: int | None = None
    while True:
        header_line = input_stream.readline()
        if not header_line:
            return None
        if header_line in (b"\r\n", b"\n"):
            break
        header_name, separator, header_value = header_line.partition(b":")
        if separator and header_name.strip().lower() == b"content-length":
            content_length = int(header_value.strip())

    if content_length is None:
        raise RuntimeError("LSP message did not contain Content-Length")

    body = bytearray()
    while len(body) < content_length:
        chunk = input_stream.read(content_length - len(body))
        if not chunk:
            raise EOFError("LSP message ended before Content-Length bytes were read")
        body.extend(chunk)
    body_bytes = bytes(body)
    return body_bytes, json.loads(body_bytes.decode("utf-8"))


def _write_lsp_body(output_stream: BinaryIO, output_lock: threading.Lock, body: bytes) -> None:
    frame = b"Content-Length: " + str(len(body)).encode("ascii") + b"\r\n\r\n" + body
    with output_lock:
        output_stream.write(frame)
        output_stream.flush()


def _write_lsp_json(output_stream: BinaryIO, output_lock: threading.Lock, message: dict[str, Any]) -> None:
    _write_lsp_body(output_stream, output_lock, json.dumps(message, separators=(",", ":")).encode("utf-8"))


def _active_build_directory(project_root: Path) -> Path:
    compilation_database_path = project_root / "compile_commands.json"
    try:
        generated_database_path = compilation_database_path.resolve(strict=True)
    except FileNotFoundError as error:
        raise RuntimeError(
            f"{compilation_database_path} is unavailable; configure Memly with CMake from the terminal first"
        ) from error

    cmake_cache_path = generated_database_path.parent / "CMakeCache.txt"
    if not cmake_cache_path.is_file():
        raise RuntimeError(
            f"the active compilation database has no adjacent CMakeCache.txt: {generated_database_path}"
        )
    return generated_database_path.parent


def _read_cmake_cache_executable(build_directory: Path, variable_name: str) -> Path:
    cmake_cache_path = build_directory / "CMakeCache.txt"
    entry_prefix = f"{variable_name}:"
    with cmake_cache_path.open(encoding="utf-8") as cmake_cache:
        for cache_line in cmake_cache:
            if not cache_line.startswith(entry_prefix):
                continue
            _, separator, executable_value = cache_line.rstrip("\n").partition("=")
            if separator and executable_value:
                executable_path = Path(executable_value)
                if not executable_path.is_absolute():
                    executable_path = build_directory / executable_path
                executable_path = executable_path.resolve()
                if executable_path.is_file() and os.access(executable_path, os.X_OK):
                    return executable_path
                raise RuntimeError(f"configured executable is unavailable: {executable_path}")
            break
    raise RuntimeError(f"{variable_name} is missing from {cmake_cache_path}; reconfigure Memly with CMake")


class _ClangdProxy:
    def __init__(self, project_root: Path, clangd_path: Path, clang_tidy_path: Path) -> None:
        self._project_root = project_root
        self._clang_tidy_path = clang_tidy_path
        self._clangd_process = subprocess.Popen(
            [
                clangd_path,
                "--enable-config",
                "--log=error",
                "--background-index",
                "--limit-results=500",
                "--completion-style=bundled",
                f"--compile-commands-dir={project_root}",
            ],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        assert self._clangd_process.stdin is not None
        assert self._clangd_process.stdout is not None
        assert self._clangd_process.stderr is not None

        self._clangd_input_lock = threading.Lock()
        self._client_output_lock = threading.Lock()
        self._diagnostics_lock = threading.Lock()
        self._clangd_diagnostics: dict[str, list[dict[str, Any]]] = {}
        self._tidy_diagnostics: dict[str, list[dict[str, Any]]] = {}
        self._document_versions: dict[str, int] = {}
        self._lint_generations: dict[str, int] = {}
        self._lint_queue: queue.Queue[tuple[str, Path, int] | None] = queue.Queue()
        self._stopping = threading.Event()

        self._server_thread = threading.Thread(target=self._forward_server_messages, name="clangd-output", daemon=True)
        self._stderr_thread = threading.Thread(target=self._forward_clangd_stderr, name="clangd-stderr", daemon=True)
        self._lint_thread = threading.Thread(target=self._run_lint_jobs, name="memly-clang-tidy", daemon=True)
        self._server_thread.start()
        self._stderr_thread.start()
        self._lint_thread.start()

        self._log(f"project root is {project_root}")

    def _log(self, message: str) -> None:
        sys.stderr.write(f"memly-lsp-proxy: {message}\n")
        sys.stderr.flush()

    def _forward_clangd_stderr(self) -> None:
        assert self._clangd_process.stderr is not None
        for line in iter(self._clangd_process.stderr.readline, b""):
            sys.stderr.buffer.write(line)
            sys.stderr.buffer.flush()

    def _combined_diagnostics(self, uri: str) -> list[dict[str, Any]]:
        return list(self._clangd_diagnostics.get(uri, ())) + list(self._tidy_diagnostics.get(uri, ()))

    def _forward_server_messages(self) -> None:
        assert self._clangd_process.stdout is not None
        while not self._stopping.is_set():
            try:
                framed_message = _read_lsp_message(self._clangd_process.stdout)
            except (EOFError, json.JSONDecodeError, RuntimeError, ValueError) as error:
                self._log(f"clangd output stopped: {error}")
                return
            if framed_message is None:
                return

            body, message = framed_message
            if message.get("method") != "textDocument/publishDiagnostics":
                _write_lsp_body(sys.stdout.buffer, self._client_output_lock, body)
                continue

            params = message.get("params", {})
            uri = params.get("uri")
            diagnostics = params.get("diagnostics")
            if not isinstance(uri, str) or not isinstance(diagnostics, list):
                _write_lsp_body(sys.stdout.buffer, self._client_output_lock, body)
                continue

            with self._diagnostics_lock:
                self._clangd_diagnostics[uri] = diagnostics
                merged_params = dict(params)
                merged_params["diagnostics"] = self._combined_diagnostics(uri)
            merged_message = dict(message)
            merged_message["params"] = merged_params
            _write_lsp_json(sys.stdout.buffer, self._client_output_lock, merged_message)

    def _publish_merged_diagnostics(self, uri: str) -> None:
        with self._diagnostics_lock:
            params: dict[str, Any] = {"uri": uri, "diagnostics": self._combined_diagnostics(uri)}
            version = self._document_versions.get(uri)
            if version is not None:
                params["version"] = version
        _write_lsp_json(
            sys.stdout.buffer,
            self._client_output_lock,
            {"jsonrpc": "2.0", "method": "textDocument/publishDiagnostics", "params": params},
        )

    def _publish_log_message(self, message: str) -> None:
        _write_lsp_json(
            sys.stdout.buffer,
            self._client_output_lock,
            {
                "jsonrpc": "2.0",
                "method": "window/logMessage",
                "params": {"type": 2, "message": f"Memly clang-tidy: {message}"},
            },
        )

    def _eligible_source(self, source_path: Path) -> bool:
        if source_path.suffix.lower() not in _CXX_SOURCE_SUFFIXES:
            return False
        try:
            relative_path = source_path.relative_to(self._project_root)
        except ValueError:
            return False
        return bool(relative_path.parts) and relative_path.parts[0] in ("program", "test")

    def _schedule_lint(self, text_document: dict[str, Any]) -> None:
        uri = text_document.get("uri")
        if not isinstance(uri, str):
            return
        source_path = _path_from_uri(uri)
        if source_path is None or not self._eligible_source(source_path):
            return

        with self._diagnostics_lock:
            version = text_document.get("version")
            if isinstance(version, int):
                self._document_versions[uri] = version
            generation = self._lint_generations.get(uri, 0) + 1
            self._lint_generations[uri] = generation
        self._lint_queue.put((uri, source_path, generation))

    def _parse_tidy_diagnostics(self, source_path: Path, output: str) -> list[dict[str, Any]]:
        diagnostics: list[dict[str, Any]] = []
        canonical_source_path = source_path.resolve()
        for output_line in output.splitlines():
            match = _DIAGNOSTIC_PATTERN.match(output_line)
            if match is None:
                continue
            diagnostic_path = Path(match.group(1))
            if not diagnostic_path.is_absolute():
                diagnostic_path = source_path.parent / diagnostic_path
            try:
                if diagnostic_path.resolve() != canonical_source_path:
                    continue
            except OSError:
                continue

            line = max(int(match.group(2)) - 1, 0)
            character = max(int(match.group(3)) - 1, 0)
            severity_name = match.group(4)
            severity = 2 if severity_name == "warning" else 3 if severity_name == "note" else 1
            diagnostic: dict[str, Any] = {
                "range": {
                    "start": {"line": line, "character": character},
                    "end": {"line": line, "character": character + 1},
                },
                "severity": severity,
                "source": "Memly clang-tidy",
                "message": match.group(5).strip(),
            }
            check_name = match.group(6)
            if check_name:
                diagnostic["code"] = check_name
            diagnostics.append(diagnostic)
        return diagnostics

    def _run_lint_jobs(self) -> None:
        while not self._stopping.is_set():
            job = self._lint_queue.get()
            if job is None:
                return
            uri, source_path, generation = job
            with self._diagnostics_lock:
                if self._lint_generations.get(uri) != generation:
                    continue

            command = [
                self._clang_tidy_path,
                "--experimental-custom-checks",
                "--quiet",
                f"--config-file={self._project_root / '.clang-tidy'}",
                "--checks=-*,custom-memly-*,misc-include-cleaner",
                "--header-filter=^$",
                "-p",
                self._project_root,
                source_path,
            ]
            started_at = time.monotonic()
            try:
                completed_process = subprocess.run(
                    command,
                    cwd=self._project_root,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    text=True,
                    timeout=120,
                    check=False,
                )
                diagnostics = self._parse_tidy_diagnostics(source_path, completed_process.stdout)
            except subprocess.TimeoutExpired:
                self._publish_log_message(f"timed out after 120 seconds for {source_path.name}")
                continue
            except OSError as error:
                self._publish_log_message(f"could not start for {source_path.name}: {error}")
                continue

            with self._diagnostics_lock:
                if self._lint_generations.get(uri) != generation:
                    continue
                self._tidy_diagnostics[uri] = diagnostics
            self._publish_merged_diagnostics(uri)
            elapsed_seconds = time.monotonic() - started_at
            self._log(
                f"clang-tidy checked {source_path.relative_to(self._project_root)} in {elapsed_seconds:.2f}s "
                f"with {len(diagnostics)} diagnostic(s)"
            )
            if completed_process.returncode != 0 and not diagnostics:
                self._publish_log_message(
                    f"failed for {source_path.name} with exit code {completed_process.returncode}; see LSP messages"
                )
                self._log(completed_process.stdout.rstrip())

    def run(self) -> int:
        assert self._clangd_process.stdin is not None
        try:
            while True:
                framed_message = _read_lsp_message(sys.stdin.buffer)
                if framed_message is None:
                    break
                body, message = framed_message
                method = message.get("method")
                params = message.get("params", {})
                if method in ("textDocument/didOpen", "textDocument/didSave") and isinstance(params, dict):
                    text_document = params.get("textDocument", {})
                    if isinstance(text_document, dict):
                        self._schedule_lint(text_document)
                elif method == "textDocument/didChange" and isinstance(params, dict):
                    text_document = params.get("textDocument", {})
                    uri = text_document.get("uri") if isinstance(text_document, dict) else None
                    version = text_document.get("version") if isinstance(text_document, dict) else None
                    if isinstance(uri, str):
                        with self._diagnostics_lock:
                            if isinstance(version, int):
                                self._document_versions[uri] = version
                            self._lint_generations[uri] = self._lint_generations.get(uri, 0) + 1
                            cleared_tidy_diagnostics = self._tidy_diagnostics.pop(uri, None) is not None
                        if cleared_tidy_diagnostics:
                            self._publish_merged_diagnostics(uri)
                elif method == "textDocument/didClose" and isinstance(params, dict):
                    text_document = params.get("textDocument", {})
                    uri = text_document.get("uri") if isinstance(text_document, dict) else None
                    if isinstance(uri, str):
                        with self._diagnostics_lock:
                            self._tidy_diagnostics.pop(uri, None)
                            self._document_versions.pop(uri, None)
                            self._lint_generations.pop(uri, None)

                _write_lsp_body(self._clangd_process.stdin, self._clangd_input_lock, body)
                if method == "exit":
                    break
        except (BrokenPipeError, EOFError, json.JSONDecodeError, RuntimeError, ValueError) as error:
            self._log(f"client connection stopped: {error}")
        finally:
            self.close()
        return self._clangd_process.returncode or 0

    def close(self) -> None:
        if self._stopping.is_set():
            return
        self._stopping.set()
        self._lint_queue.put(None)
        if self._clangd_process.poll() is None:
            self._clangd_process.terminate()
            try:
                self._clangd_process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                self._clangd_process.kill()
                self._clangd_process.wait()


def _create_argument_parser() -> argparse.ArgumentParser:
    argument_parser = argparse.ArgumentParser(description=__doc__)
    argument_parser.add_argument("--project-root", required=True, type=Path)
    return argument_parser


def main() -> int:
    arguments = _create_argument_parser().parse_args()
    try:
        project_root = arguments.project_root.expanduser().resolve(strict=True)
        active_build_directory = _active_build_directory(project_root)
        clangd_path = _read_cmake_cache_executable(active_build_directory, _CMAKE_CLANGD_VARIABLE)
        clang_tidy_path = _read_cmake_cache_executable(active_build_directory, _CMAKE_CLANG_TIDY_VARIABLE)
        return _ClangdProxy(project_root, clangd_path, clang_tidy_path).run()
    except (OSError, RuntimeError, ValueError) as error:
        print(f"memly-lsp-proxy: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
