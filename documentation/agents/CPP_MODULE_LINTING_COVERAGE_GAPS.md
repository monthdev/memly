# C++ Module Linting Coverage Gaps

This document records the observed linting differences between the C++ module
architecture and the header architecture at `main` revision
`8d86f219da150c9a870827a5240003ca6738d07a`.

## Retained Coverage

- Clang compiler diagnostics, including `-Weverything`, `-pedantic`, and
  `-Werror`, cover every authored `.cppm` and `.cpp`.
- The configured Clang-Tidy checks and `custom-memly-*` YAML checks cover every
  authored exact named-module interface and implementation unit after the
  required BMIs exist.
- clangd parses and diagnoses exact named-module interfaces and implementation
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
- Neither clangd nor Clang-Tidy diagnoses an unused named-module import.
- clangd's native missing-include diagnostics and
  `misc-include-cleaner.MissingIncludes` are disabled because they treat
  declarations supplied by named-module imports as missing textual headers.
- `readability-redundant-declaration` is disabled because imported libc++ BMIs
  produce false duplicate global allocation-function diagnostics.
- clangd 22 can assign distinct internal identities to identical library
  declarations repeated through a module interface's and implementation unit's
  global module fragments. This produces a false ODR warning for ICU
  declarations in `HumanTextInput.cpp` and cascading false type errors for
  libc++ containers in `DeckForestSnapshotIndex.cpp`; the compiler and
  Clang-Tidy accept both units. No diagnostic suppression is configured. The
  upstream clangd defect is tracked by
  [LLVM issue #196678](https://github.com/llvm/llvm-project/issues/196678).
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
  updates custom checks for `.cppm` interfaces and `module/Internal/` modules.
- `.clangd` disables module-incompatible missing-include diagnostics while
  retaining strict unused physical-header diagnostics.
- `CMakeLists.txt` replaces the header-era IWYU and independent-header gates
  with the BMI-aware `memly-clang-tidy` target. Each static component target
  publishes its direct `module/` interface file set and privately owns its
  `module/Internal/` interface file set. Exported-interface imports form public
  component edges; implementation-only imports form explicitly declared private
  edges. Each static component exposes only its public link interface, while the
  executable explicitly supplies the complete static-link closure. CMake rejects
  cross-target imports of private module file sets and does not make a private
  dependency's public BMIs available to indirect component consumers. File
  stamps are scheduled through a bounded Ninja pool. Each implementation stamp
  follows its exact interface and recursive imported-interface closure; each
  interface stamp follows only its recursive imported-interface closure. A
  content-stable compilation-database snapshot avoids nonsemantic full-gate
  invalidation.
- `.vscode/settings.json` recognizes `.cppm`, removes IWYU, and runs the reduced
  editor Clang-Tidy set separately for module interfaces and implementation
  units against the root compilation database that CMake Tools refreshes from
  the active configure preset. Native extensions own supported editor
  formatting; Lint Runner remains the adapter for experimental YAML checks and
  CMake commands.
- `.codex/hooks/lint_build_test_project.sh` invokes the default build, whose
  required `memly-clang-tidy` target runs after compilation has generated every
  importer module map.
- `.codex/hooks/check_repository_format.sh`,
  `.github/workflows/CheckRepositoryFormat.yml`, and `.prettierignore` replace
  authored `.hpp` formatting coverage with `.cppm` coverage.
- `tool/verify_source_extensions.py`, invoked by CMake, replaces the authored
  header/source extension policy with the `.cppm` and `.cpp` module policy.

The module branch deletes `.iwyu.imp` and `tool/run_include_what_you_use.py`.
IWYU is neither invoked by CMake nor exposed through Lint Runner on this branch.

## Resource Constraint

Each Clang-Tidy process can load a large transitive PCM closure. An unbounded
full-tree run attempted to use all ten logical CPUs on a 16 GiB development
machine and exhausted memory. `MEMLY_CLANG_TIDY_JOBS` controls the Ninja lint
pool and the opt-in profiler. It defaults to four and accepts any positive
integer so the developer can tune concurrency for the current machine. Ninja's
overall worker limit still constrains the build path; the profiler uses the
configured value directly.

Before exact named modules, the editor's reduced `custom-memly-*` and
unused-physical-header check set took 8.14 seconds for an exported service
interface and 11.07 seconds for the deepest composition implementation unit.
Each Lint Runner analysis starts one process, while clangd remains responsible
for built-in diagnostics. Lint Runner has no project-wide process limit, so
overlapping editor analyses must be assessed separately from the bounded
full-tree gate.

These gaps weaken direct-import and textual-header hygiene, not compiler type
checking, authored-source warning coverage, or the module targets' binary ABI.
