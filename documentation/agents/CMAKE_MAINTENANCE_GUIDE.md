# Memly CMake Maintenance Guide

## Maintaining This Guide

Follow [`META_GUIDE.md`](META_GUIDE.md) for the shared structure and routing
requirements of every guide.

Keep this guide scoped to stable rules for Memly's CMake files and tools that
CMake invokes, configures, or uses to validate generated build state. Put rules
for authored project code and tools that directly enforce code spelling or
structure in [`CODING_GUIDE.md`](CODING_GUIDE.md). Put investigation history,
measurements, enforcement coverage changes and gaps, and other patch evidence in
[`PATCH_ARTIFACTS_GUIDE.md`](PATCH_ARTIFACTS_GUIDE.md). Put optional local hook
orchestration in
[`HOOKS_RECOMMENDATION_GUIDE.md`](recommendation/HOOKS_RECOMMENDATION_GUIDE.md),
and put editor diagnostic integration in
[`LINTING_SETUP_RECOMMENDATION_GUIDE.md`](recommendation/LINTING_SETUP_RECOMMENDATION_GUIDE.md).

Update this guide only when a stable CMake rule, CMake-owned tool
responsibility, or configuration-output policy changes.

State each CMake rule normatively and cite its narrowest applicable `memly_*()`
helper or validator. Do not duplicate implementation inventories, current
control flow, tool versions, editor behavior, or CI behavior. Prune superseded
prose together with the corresponding CMake rule or enforcement change.

All applicable configured gates must pass.

## Organization and Discovery

Give each CMake glob variable one folder and do not combine patterns. Recurse
only for repository-wide policy discovery. Order globs and target sources by
source-tree order.

Declare a target's globs, include roots, resources, and direct dependencies
beside its source tree. Do not add a forwarding-only architectural
`CMakeLists.txt`. Keep repository-wide policy at the root, add target
directories from their nearest owner, and register modules and component
dependencies through `memly_add_component()` and
`memly_link_component_dependencies()`.

Treat an explicitly configured `ICU_ROOT` as authoritative. Every discovered ICU
include directory and library must resolve below that root
(`memly_validate_explicit_icu_root()`).

## Component and Module Targets

Map all named modules owned by a component to that component's single static
target. Put public module interfaces in its public `CXX_MODULES` file set and
internal module interfaces in its private `CXX_MODULES` file set. Do not add
primary routing modules, re-export umbrellas, or module partitions. Only
implementation units in the owning component may import an `Internal` module; a
public interface may not import one. CMake's module file-set visibility and
component-import validation enforce this owner-only boundary
(`memly_add_component()`, `memly_validate_component_import_graph()`).

Declare component target relationships explicitly rather than deriving target
edges from imports. Use `PUBLIC` when a public module interface imports the
dependency and `PRIVATE` for an implementation-only import. Curate each static
component's outward link interface to its public dependencies so private
dependency BMIs do not reach indirect consumers, and explicitly link the
complete static closure into the final executable. Every active cross-component
Memly import requires a direct component edge, and every direct Memly component
edge requires an active import. Make an edge `PUBLIC` exactly when an exported
interface imports that component; otherwise make it `PRIVATE`. A transitively
available BMI does not grant permission to import its module. Keep a third-party
dependency private unless clients must name its API directly
(`memly_link_component_dependencies()`,
`memly_validate_component_import_graph()`,
`memly_link_complete_component_closure()`).

## Configuration Output

Report only effective cache settings that materially change Memly's build or
lint gate:

- `-DCMAKE_BUILD_TYPE=<value>` for a single-configuration generator, or
  `-DCMAKE_CONFIGURATION_TYPES="<value;...>"` for a multi-configuration
  generator;
- `-DMEMLY_CLANG_TIDY_JOBS=<value>`; and
- `-DICU_ROOT=<value>`, or `<unset; automatic discovery>` when CMake chooses the
  ICU root.

The `-DVARIABLE=value` spelling mirrors an actual CMake command-line cache
assignment. Do not add spaces around `=`, which would turn the display into
three shell arguments rather than a copyable assignment.

Keep the configuration-check count and its elapsed time on one status line so
the duration cannot be mistaken for the configuration-value summary:

```text
-- Memly configuration checks (<count> module units) | Elapsed time: <duration>
```

Do not dump the complete CMake cache. It duplicates CMake's discovery output,
includes incidental toolchain and host paths, and obscures the settings that a
developer is expected to control. Add another summary line only when a new cache
setting or automatic choice materially changes generated code, diagnostics,
dependencies, packaging, or runtime behavior.
