# C++ Module Linting Coverage Gaps

This document records the observed linting differences between the C++ module
architecture and the header architecture at `main` revision
`8d86f219da150c9a870827a5240003ca6738d07a`.

## Retained Coverage

- Clang compiler diagnostics, including `-Weverything`, `-pedantic`, and
  `-Werror`, cover every authored `.cppm` and `.cpp`.
- The configured Clang-Tidy checks and `custom-memly-*` YAML checks cover every
  authored `.cppm` and `.cpp` after the required BMIs exist.
- clangd parses and diagnoses module interfaces, partitions, and implementation
  units against the CMake-generated BMI graph.
- A missing import remains a compiler error when the required declaration is not
  otherwise reachable.
- `misc-include-cleaner` continues to diagnose unused physical standard-library
  and third-party headers in global module fragments.

## Coverage Gaps

- clangd and standalone Clang-Tidy cannot analyze an importer after a clean
  configure until CMake has compiled its imported module interfaces and produced
  compatible BMIs and module maps.
- Consumer analysis can temporarily use the last-built dependency BMI after an
  imported interface changes. The interface must be rebuilt before consumer
  diagnostics reflect the change.
- The default build compiles and links before the full-tree gate because CMake
  generates executable-importer module maps during target compilation. A lint
  failure still fails the overall build, although the executable may already
  have been produced.
- Neither clangd nor Clang-Tidy diagnoses an unused named-module import or
  requires the most-direct import when a declaration is already reachable
  through a re-export.
- clangd's native missing-include diagnostics and
  `misc-include-cleaner.MissingIncludes` are disabled because they treat
  declarations supplied by named-module imports as missing textual headers.
- `readability-redundant-declaration` is disabled because imported libc++ BMIs
  produce false duplicate global allocation-function diagnostics.
- Include What You Use 0.26 cannot analyze the imported graph with the Clang 22
  BMI and module-map arguments: it terminates with `SIGSEGV`. Removing those
  arguments prevents the crash but makes the imports unavailable, so IWYU is not
  a project gate on the module branch.
- clangd's explicit `--experimental-modules-support` mode parsed the project but
  took 33.82 seconds for `main.cpp`, compared with 1.29 seconds using the normal
  CMake project-BMI path. The experimental mode is therefore not enabled.

## Lint Configuration Changes

The module branch changes these lint- and source-policy files:

- `.clang-tidy` disables module-incompatible missing-include and redundant
  declaration diagnostics, retains unused physical-header diagnostics, and
  updates custom checks for `.cppm` interfaces and `source/` private helpers.
- `.clangd` disables module-incompatible missing-include diagnostics while
  retaining strict unused physical-header diagnostics.
- `CMakeLists.txt` replaces the header-era IWYU and independent-header gates
  with the BMI-aware `memly-clang-tidy` target over every authored `.cppm` and
  `.cpp`, bounded by `MEMLY_CLANG_TIDY_JOBS`.
- `.vscode/settings.json` recognizes `.cppm`, removes IWYU, and runs the reduced
  editor Clang-Tidy set separately for module interfaces and implementation
  units.
- `.codex/hooks/verify-project-integrity.sh` invokes the default build, whose
  required `memly-clang-tidy` target runs after compilation has generated every
  importer module map.
- `.codex/hooks/enforce-formatting.sh`,
  `.github/workflows/CheckRepositoryFormat.yml`, `.prettierignore`, and
  `tool/verify_source_extensions.py` replace authored `.hpp` coverage with
  `.cppm` coverage.

The module branch deletes `.iwyu.imp` and `tool/run_include_what_you_use.py`.
IWYU is neither invoked by CMake nor exposed through Lint Runner on this branch.

## Resource Constraint

Each Clang-Tidy process can load a large transitive PCM closure. An unbounded
full-tree run attempted to use all ten logical CPUs on a 16 GiB development
machine and exhausted memory. The full-tree gate therefore permits at most six
concurrent Clang-Tidy processes independently of normal Ninja build parallelism.
The same cap applies when the gate is invoked explicitly or as part of the
default build.

The editor's reduced `custom-memly-*` and unused-physical-header check set took
8.14 seconds for an exported service partition and 11.07 seconds for the deepest
composition implementation unit. Each saved file starts one Lint Runner process,
while clangd remains responsible for built-in diagnostics. Lint Runner has no
project-wide process limit, so automatic analyses of different files can
overlap; that editor behavior must be assessed separately from the bounded
full-tree gate.

These gaps weaken direct-import and textual-header hygiene, not compiler type
checking, authored-source warning coverage, or the module targets' binary ABI.
