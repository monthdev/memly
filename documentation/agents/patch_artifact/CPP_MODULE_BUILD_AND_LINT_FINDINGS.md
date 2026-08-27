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
- clangd parses the same CMake-produced module graph.
- `misc-include-cleaner` still diagnoses unused standard-library and third-party
  physical headers in global module fragments.
- A missing import remains a compiler error when the declaration is not already
  reachable through the implementation unit's primary module interface.

## Known Gaps

- A clean configure does not produce BMIs. clangd and standalone Clang-Tidy
  cannot analyze an importer until CMake builds the imported interfaces.
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
- clangd 22 can assign different internal identities to identical declarations
  repeated through global module fragments. This produces false ICU and libc++
  ODR/type diagnostics while the compiler and Clang-Tidy accept the same units.
  The upstream defect is tracked by
  [LLVM issue #196678](https://github.com/llvm/llvm-project/issues/196678).
- Include What You Use 0.26 terminates with `SIGSEGV` when given the Clang 22
  BMI and module-map arguments. Removing those arguments makes imports
  unavailable, so IWYU is not a module-branch gate.
- clangd's explicit experimental module mode took 33.82 seconds to parse
  `main.cpp`, compared with 1.29 seconds through the ordinary CMake BMI path. It
  remains disabled.

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

The current tree contains 88 module units plus `main.cpp`, for 89 Clang-Tidy
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
