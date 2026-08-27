# Memly Agentic Coding Hook Recommendations

## Maintaining This Guide

Follow [`META_GUIDE.md`](../META_GUIDE.md) for the shared structure and routing
requirements of every guide.

Keep this guide scoped to optional local lifecycle-hook patterns for agentic
coding clients. Put the rules those hooks invoke in
[`CODING_GUIDE.md`](../CODING_GUIDE.md) or
[`CMAKE_MAINTENANCE_GUIDE.md`](../CMAKE_MAINTENANCE_GUIDE.md), editor diagnostic
integration in
[`LINTING_SETUP_RECOMMENDATION_GUIDE.md`](LINTING_SETUP_RECOMMENDATION_GUIDE.md),
and investigation evidence in
[`PATCH_ARTIFACTS_GUIDE.md`](../PATCH_ARTIFACTS_GUIDE.md).

Update this guide only when the recommended lifecycle, sequencing, or
portability of local hooks changes. Do not make a particular agent client's hook
schema or ignored local configuration part of Memly's required repository
interface.

## Recommended Completion Hook

Use one completion hook that stops on the first failed phase and runs these
independently invocable phases in order:

1. Format the working tree in write mode.
2. Verify agents-documentation structure and content policy.
3. Configure only when needed, then invoke the aggregate lint, build, and future
   test path.

Run formatting first because it may change source bytes; every later check must
inspect the formatted result. Keep a separate check-only formatting mode for
humans and CI. Treat an empty test discovery as no tests executed, not as tests
passing.

Delegate each phase to a responsibility-shaped script. Keep enforcement in the
checked-in formatter, documentation verifiers, CMake targets, and CI workflows;
the hook should only compose those authorities. Run expensive full-project
analysis at completion rather than after every edit or patch operation, and rely
on the build graph's incremental invalidation.

Resolve executables through `PATH` or the active CMake cache, never through an
absolute machine path or an implicit network installation. Reconfigure when the
selected preset inputs change, not on every completion. Keep output quiet on
success and actionable on failure.

Local hooks may format files, but must not stage, commit, reset, revert, or
otherwise alter Git state. Keep client-specific hook files ignored; this guide
shares the reusable pattern rather than a mandatory personal setup.
