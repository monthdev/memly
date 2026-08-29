# C++ Module Build and Lint Findings

This artifact follows the evidence policy in
[`PATCH_ARTIFACTS_GUIDE.md`](../PATCH_ARTIFACTS_GUIDE.md).

This artifact records the enforcement retained by Memly's C++ module build, the
known tooling gaps, and the changes that produced the current gate. It is a
design artifact, not a second source of coding rules. CMake's relevant upstream
model is documented in
[`cmake-cxxmodules(7)`](https://cmake.org/cmake/help/v4.4/manual/cmake-cxxmodules.7.html).

## Retained Enforcement

- The compiler applies `-Weverything`, `-pedantic`, and `-Werror` to every
  authored `.cppm` and `.cpp`.
- CMake validates source extensions, module declarations, import ordering,
  internal-module ownership, direct component edges, and literal physical
  includes during configuration.
- Each component exposes public module interfaces through a public `CXX_MODULES`
  file set and owns its `Internal` interfaces through a private file set. The
  final executable supplies the complete static archive closure.
- Clang-Tidy analyzes every authored module interface and implementation unit
  after its required BMIs and module maps exist. Per-file stamps invalidate an
  edited unit and its actual import dependents instead of the whole tree.
- Ordinary clangd parses the same CMake-produced module graph. LLVM 23's
  experimental mode instead builds a separate content-addressed BMI cache.
- `misc-include-cleaner` still diagnoses unused standard-library and third-party
  physical headers in global module fragments.
- A missing import remains a compiler error when the declaration is not already
  reachable through the implementation unit's primary module interface.

## Known Gaps

- A clean configure does not produce CMake BMIs. Ordinary clangd and standalone
  Clang-Tidy cannot analyze an importer until CMake builds the imported
  interfaces. LLVM 23 experimental clangd can synthesize its own BMIs, but
  Clang-Tidy cannot consume that private cache.
- An importer can temporarily be analyzed against the last-built dependency BMI
  after an interface edit. Rebuilding the interface refreshes consumer
  diagnostics.
- The default build must compile module targets before the complete Clang-Tidy
  gate because CMake generates importer module maps during compilation. A lint
  failure still fails the build, although the executable may already have been
  linked.
- Neither clangd nor Clang-Tidy diagnoses an unused named-module import.
- A module implementation unit implicitly imports its primary interface.
  Existing tools therefore cannot require a repeated explicit import for a
  declaration already reachable through that interface. CMake's lint-stamp graph
  controls scheduling and invalidation, not semantic name lookup. Memly's
  explicit-import rule remains a manual review rule unless a semantic source
  analyzer is introduced.
- Native missing-include diagnostics are disabled because they misinterpret
  declarations supplied by named-module imports as missing textual headers.
  Redundant-declaration analysis is also disabled because imported libc++ BMIs
  produce false duplicate allocation-function diagnostics.
- Ordinary clangd 23.1.0 still assigns different internal identities to
  identical declarations repeated through global module fragments. This produces
  false ICU and libc++ ODR/type diagnostics while the compiler and Clang-Tidy
  accept the same units. The similar minimal reproduction in
  [LLVM issue #196678](https://github.com/llvm/llvm-project/issues/196678) was
  closed after a newer development snapshot fixed that reproduction, but the two
  Memly cases remain reproducible in the 23.1.0 ordinary path.
- Include What You Use 0.26 terminates with `SIGSEGV` when given the Clang 22
  BMI and module-map arguments. Removing those arguments makes imports
  unavailable, so IWYU is not a module-branch gate.
- LLVM 23's experimental module mode fixes the two known false-diagnostic cases
  in the actual LSP path, but not in `clangd --check`. It remains opt-in because
  it duplicates CMake's BMI storage and has materially higher cold-start,
  memory, and edit costs.

## Changed Files and Enforcement Authorities

- [`.clang-tidy`](../../../.clang-tidy) sets
  `misc-include-cleaner.MissingIncludes` to `false`, leaves
  `misc-include-cleaner.UnusedIncludes` enabled, disables
  `readability-redundant-declaration`, and adapts Memly's custom checks to
  `.cppm` interfaces and `module/Internal/` ownership.
- [`.clangd`](../../../.clangd) sets `Diagnostics.MissingIncludes` to `None`,
  retains `Diagnostics.UnusedIncludes: Strict`, and removes `custom-memly-*`
  from clangd's interactive pass because the complete Clang-Tidy gate runs them
  separately.
- [`memly_lsp_proxy.py`](../../../tool/lsp/memly_lsp_proxy.py) exposes
  `--experimental-modules-support` as an explicit editor tradeoff without
  changing the ordinary CMake-BMI default.
- [`CMakeLists.txt`](../../../CMakeLists.txt) owns the active implementation:
  `memly_verify_source_extensions()` restricts authored C++ file types;
  `memly_get_module_interface_metadata()` and
  `memly_validate_module_implementation()` validate interface/implementation
  pairing; `memly_validate_module_unit_structure()` validates declarations and
  imports; `memly_add_component()` registers public and private `CXX_MODULES`
  file sets; `memly_link_component_dependencies()` and
  `memly_validate_component_import_graph()` validate imports against direct
  public/private component edges; `memly_link_complete_component_closure()`
  supplies the final static archive closure; and the per-input
  `memly-clang-tidy` stamps follow each unit's actual imported-interface
  closure.
- [`verify_header_includes.py`](../../../tool/cmake/verify_header_includes.py)
  enforces literal physical includes and the deprecated/redundant C-header
  policy over CMake's discovered `.cppm` and `.cpp` set.
- [`.github/workflows/CheckRepositoryFormat.yml`](../../../.github/workflows/CheckRepositoryFormat.yml)
  and [`.prettierignore`](../../../.prettierignore) apply formatting policy to
  `.cppm` and `.cpp` rather than the removed authored-header layout.
- [`.gitignore`](../../../.gitignore) excludes personal editor workspaces and
  agent lifecycle hooks while leaving editor-neutral compiler, formatter, CMake,
  and CI policy tracked.
- `.iwyu.imp` and `tool/run_include_what_you_use.py` were deleted after Include
  What You Use 0.26 crashed on the required Clang 22 BMI/module-map arguments.
  Header-era independent-header gates were likewise replaced by the BMI-aware
  complete Clang-Tidy gate and its content-stable compilation-database snapshot.
- Exact named modules replaced umbrella interfaces and partitions. Component
  targets retain binary ownership while CMake's scanned per-unit module maps
  avoid exposing every sibling BMI to each consumer.

## Resource and Build Findings

The current tree contains 87 module units plus `main.cpp`, for 88 Clang-Tidy
inputs. Each lint worker can load a large transitive PCM closure. Four workers
are the default on the 16 GiB development machine; `MEMLY_CLANG_TIDY_JOBS`
accepts any positive integer for controlled machine-specific comparisons.

A clean CMake 4.4 and Clang 22 scratch probe confirmed that module-scanned
sources are excluded from CMake unity batches, matching CMake's documented
[`UNITY_BUILD`](https://cmake.org/cmake/help/v4.4/prop_tgt/UNITY_BUILD.html)
behavior. Every active Memly C++ source is module-scanned, so enabling unity
builds would not combine any authored source and would provide no current
build-speed benefit.

These gaps weaken direct-import and textual-header hygiene; they do not weaken
compiler type checking, authored-source warning coverage, or the component
targets' binary ABI.

## LLVM 23 Experimental clangd Probe

The August 29, 2026 probe used Homebrew clangd 23.1.0, CMake 4.4.3, the Debug
compilation database, `--background-index=false`, and a fresh clangd process per
measurement. Times are wall-clock times from `/usr/bin/time`; memory is maximum
resident set size. `clangd --check` exercises additional per-token checks and
does not match the actual LSP diagnostic path, so editor correctness was
verified separately through `initialize`, `didOpen`, and every diagnostic
publication observed for five seconds.

| Input and state                                           | Ordinary CMake BMI | Experimental clangd BMI | Result                                           |
| --------------------------------------------------------- | -----------------: | ----------------------: | ------------------------------------------------ |
| `DeckForestSnapshotIndex.cpp`, existing build, `--check`  |    1.88 s / 195 MB |         2.18 s / 218 MB | Both reported the same false libc++ type errors. |
| `HumanTextInput.cpp`, existing build, `--check`           |    0.64 s / 180 MB |         1.26 s / 216 MB | Both reported the same false ICU ODR error.      |
| `main.cpp`, existing build, cold experimental cache       |    3.52 s / 494 MB |       36.84 s / 1.82 GB | Both completed without diagnostics.              |
| `main.cpp`, existing build, warm experimental cache       |    3.52 s / 494 MB |         7.50 s / 544 MB | Both completed without diagnostics.              |
| `DeckForestSnapshotIndex.cpp`, configure only, actual LSP |        unavailable |                  3.31 s | Experimental LSP published no diagnostics.       |
| `HumanTextInput.cpp`, configure only, actual LSP          |        unavailable |                  2.81 s | Experimental LSP published no diagnostics.       |
| `main.cpp`, configure only, actual LSP                    |        unavailable |                 25.55 s | Experimental LSP published no diagnostics.       |

With existing generated state and a warm experimental cache, actual LSP
diagnostics arrived in 0.87 seconds for `DeckForestSnapshotIndex.cpp`, 0.62
seconds for `HumanTextInput.cpp`, and 5.03 seconds for `main.cpp`. Reanalyzing a
same-process whitespace edit took about 1.00, 0.67, and 2.00 seconds
respectively. A cold actual-LSP `main.cpp` open took 24.07 seconds and peaked at
1.73 GB. This matches the
[LLVM 23 module-support release notes](https://releases.llvm.org/23.1.0/tools/clang/tools/extra/docs/ReleaseNotes.html#improvements-to-c-20-modules-support):
clangd now caches its built modules across invocations, but deliberately
disables its normal preamble optimization for translation units that use
modules.

The configure-only compilation database contained neither `.pcm` nor `.modmap`
files. Experimental clangd built private BMIs and produced correct LSP
diagnostics, although its whole-database dependency scan logged failures for
not-yet-generated Qt/QML/RCC sources. Running standalone Clang-Tidy afterward
still failed on the absent CMake `.modmap` and unavailable named module. The
experimental LSP did emit an intentionally introduced
`readability-container-size-empty` diagnostic from clangd's integrated built-in
Clang-Tidy pass before compilation. It therefore provides pre-build interactive
built-in diagnostics for opened files, but cannot replace
`memly-source-build-ready`, the content-stable Clang-Tidy compilation database,
the YAML custom-check driver, or the per-unit lint stamps.

The older LLVM 22 experiment measured 33.82 seconds for experimental `main.cpp`
parsing versus 1.29 seconds through CMake's BMI path. LLVM 23's cross-invocation
cache substantially improves repeat use and fixes the actual LSP false
diagnostics, but does not justify pruning the CMake module or lint pipeline.
