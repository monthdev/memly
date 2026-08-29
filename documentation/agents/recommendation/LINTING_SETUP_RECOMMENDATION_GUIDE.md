# Memly Editor Linting Setup Recommendations

## Maintaining This Guide

Follow [`META_GUIDE.md`](../META_GUIDE.md) for the shared structure and routing
requirements of every guide.

Keep this guide scoped to optional in-editor clangd and standalone Clang-Tidy
integration. Put code rules in [`CODING_GUIDE.md`](../CODING_GUIDE.md), CMake's
mandatory lint graph in
[`CMAKE_MAINTENANCE_GUIDE.md`](../CMAKE_MAINTENANCE_GUIDE.md), local agent
lifecycle orchestration in
[`HOOKS_RECOMMENDATION_GUIDE.md`](HOOKS_RECOMMENDATION_GUIDE.md), and measured
tooling behavior in [`PATCH_ARTIFACTS_GUIDE.md`](../PATCH_ARTIFACTS_GUIDE.md).

Update this guide only when the recommended editor split or a compatibility
quirk changes. Do not turn editor-specific extension names, settings schemas, or
machine paths into repository requirements.

## Recommended Diagnostic Split

Use clangd as the long-lived semantic engine for parsing, completion, compiler
diagnostics, built-in Clang-Tidy checks, and native unused-include diagnostics.
Point it at the repository-root `compile_commands.json`, which CMake publishes
as a symbolic link to the most recently configured build tree. Never hard-code a
build-directory path in editor configuration.

Supplement clangd with one serialized standalone Clang-Tidy invocation for the
checks clangd does not execute from Memly's YAML `CustomChecks`. Keep that pass
narrow: `custom-memly-*` plus `misc-include-cleaner` for physical headers in
global module fragments. Do not execute the complete `.clang-tidy` selection a
second time inside the editor.

An editor adapter should run the supplemental pass on open and saved content,
not every text change; discard an older result when a newer document generation
exists, clear its diagnostics when the document changes, and merge its results
with clangd diagnostics. Parse diagnostics from a nonzero Clang-Tidy result
before classifying it as an invocation failure because Memly promotes findings
to errors. [`tool/lsp/memly_lsp_proxy.py`](../../../tool/lsp/memly_lsp_proxy.py)
implements this model for editors that accept a custom LSP command.

## C++ Module Quirks

- Configure before opening the editor, then build once when using clangd's
  ordinary explicit-BMI path. A clean configuration publishes the compilation
  database but does not create the BMIs and module maps that path consumes.
  Rebuild an edited interface before trusting importer diagnostics because
  clangd can temporarily analyze against the last-built dependency BMI.
- LLVM 23's experimental module builder is an opt-in correctness workaround for
  ordinary clangd's false ICU and libc++ module diagnostics. Pass
  `--experimental-modules-support` to
  [`memly_lsp_proxy.py`](../../../tool/lsp/memly_lsp_proxy.py) to use it. The
  actual LSP path correctly analyzed `DeckForestSnapshotIndex.cpp` and
  `HumanTextInput.cpp` from a configure-only tree, but it maintains a second BMI
  cache and has materially higher cold-start, memory, and edit costs. See the
  [recorded LLVM 23 probe](../patch_artifact/CPP_MODULE_BUILD_AND_LINT_FINDINGS.md#llvm-23-experimental-clangd-probe)
  before making it an editor default.
- Experimental clangd can run its integrated built-in Clang-Tidy checks for an
  opened file before CMake compiles the module graph. It does not make
  standalone Clang-Tidy or Memly's YAML custom checks pre-build capable.
  Standalone Clang-Tidy still consumes CMake's generated module maps and BMIs,
  so the mandatory gate must remain after the module build. `clangd --check`
  also still reports the known false module diagnostics under the experimental
  flag even though the long-lived LSP path does not.
- Associate `.cppm` with C++.
- Missing textual-include diagnostics are disabled because named-module imports
  produce false suggestions. Unused physical includes remain checked.
- Neither clangd nor Clang-Tidy diagnoses an unused named-module import. A
  module implementation also receives its primary interface implicitly, so the
  tools cannot require a redundant explicit import for names already reachable
  there.
- Ordinary clangd can report false duplicate-type or ODR diagnostics when
  third-party declarations repeat through global module fragments. When the
  compiler and the mandatory Clang-Tidy gate accept the same unit, confirm the
  known tooling gap or use the experimental LLVM 23 mode before changing valid
  code to satisfy the editor.

Standalone Clang-Tidy is a batch translation-unit analysis and is materially
heavier than clangd's incremental path. Do not overlap an editor-triggered pass
with the full CMake Clang-Tidy gate when diagnosing cancellation, stale results,
or memory pressure.
