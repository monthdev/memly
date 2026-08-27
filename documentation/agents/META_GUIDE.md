# Memly Meta Guide

## Maintaining This Guide

Keep this guide scoped to the shared structure, scope declaration, and routing
rules for guide documents under `documentation/agents/`. Put stable rules for
Memly project code files and their direct enforcement tools in
[`CODING_GUIDE.md`](CODING_GUIDE.md). Put stable rules for CMake files and tools
that CMake invokes, configures, or uses to validate generated build state in
[`CMAKE_MAINTENANCE_GUIDE.md`](CMAKE_MAINTENANCE_GUIDE.md). Put rules governing
patch artifacts in [`PATCH_ARTIFACTS_GUIDE.md`](PATCH_ARTIFACTS_GUIDE.md). Put
optional local agent lifecycle-hook and editor diagnostic integration
recommendations under [`recommendation/`](recommendation/).

Update this guide only when the shared requirements for authoring a guide
change. Do not add an individual guide's subject rules or a current guide
inventory here.

## Required Guide Structure

Begin every `*_GUIDE.md` under `documentation/agents/` with a title followed
immediately by `## Maintaining This Guide` as its first level-two section. Give
that section substantive prose before the next level-two section
(`tool/agents_documentation/verify_meta_guide_structure.py`).

The maintenance section defines the files and tools the guide controls, begins a
separate `Update this guide only ...` paragraph that limits when the guide may
change, and redirects every adjacent responsibility to the appropriate guide.
Keep the remainder of the guide within that declared scope.
