# Memly Patch Artifacts Guide

## Maintaining This Guide

Follow [`META_GUIDE.md`](META_GUIDE.md) for the shared structure and routing
requirements of every guide.

Keep this guide scoped to rules for documents under
`documentation/agents/patch_artifact/`. Put stable rules for authored project
code and tools that directly enforce code rules in
[`CODING_GUIDE.md`](CODING_GUIDE.md). Put stable CMake-file rules and the
maintenance of tools that CMake invokes, configures, or uses to validate
generated build state in
[`CMAKE_MAINTENANCE_GUIDE.md`](CMAKE_MAINTENANCE_GUIDE.md). Put optional local
hook orchestration in
[`HOOKS_RECOMMENDATION_GUIDE.md`](recommendation/HOOKS_RECOMMENDATION_GUIDE.md),
and put editor diagnostic integration in
[`LINTING_SETUP_RECOMMENDATION_GUIDE.md`](recommendation/LINTING_SETUP_RECOMMENDATION_GUIDE.md).

Treat a patch artifact as an evidence record for a particular investigation or
change, not as an additional code or CMake rule authority.

Update this guide only when the evidence discipline shared by patch artifacts
changes. Do not add the findings of an individual patch here.

## Evidence Priority

Prioritize the evidence needed to reproduce and audit mechanical enforcement:

1. Coverage added, removed, narrowed, widened, or left as a known gap.
2. Concrete authorities, including repository-relative files, exact CMake
   helpers or targets, `custom-memly-*` checks, scripts under `tool/`, and the
   relevant symbols or configuration keys.
3. Reproduction evidence, including commands, tool versions, source and
   configuration hashes, process counts, input counts, measurements, and whether
   verification was clean, incremental, or artifact-based.
4. Primary upstream documentation, issue, and source-code citations that
   establish behavior outside Memly.

Distinguish direct observations from inferences. Preserve failed probes,
negative results, counterintuitive observations, and superseded configurations
when they explain a decision or prevent the same false conclusion from being
repeated. In particular, do not delete provenance, citations, hashes, or
measured comparisons merely to shorten a document.

Tidy repetitive narrative, obsolete recommendations, and conclusions that a
newer controlled result directly supersedes. When a newer result is not
controlled against an older one, retain both and state the changed variables or
unknowns.
