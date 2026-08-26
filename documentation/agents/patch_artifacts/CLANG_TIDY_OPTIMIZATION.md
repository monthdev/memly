# Clang-Tidy Optimization

This note tracks evidence and decisions for curating Memly's Clang-Tidy gate. Do
not change diagnostic coverage from timing intuition alone; profile the
uncurated check set, change one enforcement category, and compare the same input
set and process count.

## Baseline Scope

The mandatory gate analyzes every implementation unit and every exported or
private exact named-module interface. Each `.mpp` owns its declarations; the
project has no primary routing interfaces or module partitions.

The gate creates one stamp per lint input for scheduling and progress. A stamp
depends on its own file and the recursive exact-interface closure it imports. An
implementation unit also depends on its corresponding interface. An
implementation-only change therefore invalidates only that file; an interface
change invalidates its implementation and actual importers without invalidating
unrelated interfaces in the same component folder. Each static component target
owns all of its exact named-module interfaces, and the completed component
targets remain order-only prerequisites so every required BMI and module map
exists before linting. A content-stable compilation-database snapshot prevents
an otherwise identical CMake regeneration from invalidating every stamp. The
configured `MEMLY_CLANG_TIDY_JOBS` value is the global Ninja pool capacity and
the profiler's worker count.

The 2026-08-22 exact-module conversion retained the same 89 declaration-bearing
lint inputs while removing the umbrella-induced BMI closure. Representative
module-map entries changed as follows:

| Input                    | Umbrella and partitions | Exact named modules |
| ------------------------ | ----------------------: | ------------------: |
| `DeckService.cpp`        |                      41 |                  17 |
| `DeckService.mpp`        |                      37 |                  15 |
| `DatabaseRuntime.cpp`    |                      14 |                   8 |
| `DatabaseRuntime.mpp`    |                       6 |                   6 |
| `ApplicationRuntime.mpp` |                      41 |                  16 |
| `main.cpp`               |                      46 |                  19 |

This measures semantic fanout, not resident memory. It establishes that leaf
analysis no longer receives every sibling BMI merely because it imports one
component capability. The later component-target consolidation retained exact
named modules and CMake's scan-derived per-translation-unit module maps while
grouping binary ownership at the component boundary. Exported-interface imports
form public component edges; implementation-only imports remain direct edges
without propagating their BMI providers to downstream components.

## Obtaining a Profile

Configure the intended process count explicitly when comparing runs:

```sh
cmake --preset macos-debug-local -DMEMLY_CLANG_TIDY_JOBS=4
```

Build the required BMIs and run the opt-in aggregate check profiler:

```sh
cmake --build --preset macos-debug-local --target memly-clang-tidy-profile
```

`run-clang-tidy` prints completed files with their durations and then a
per-check timing report. The target streams and saves the complete profiler
output without filtering it to `build/<preset>/clang-tidy-profile.txt` while
preserving the profiler's exit status. Clang-Tidy's aggregate process-time rows
can overlap under concurrency; CMake's final `Elapsed time` is the complete
command wall time. The report records its UTC start, host, logical processor
count, Git commit, branch and dirty state, configuration and
compilation-database hashes, tool version, build configuration, compiler and
linker, worker count, lint input count, and exact command before the timing
output.

Profiling instruments every check and is intentionally excluded from the
mandatory build. Compare profiles produced with the same build type, compiler,
BMIs, process count, and machine workload.

## Uncurated Baseline

The 2026-08-20 macOS Debug baseline used LLVM Clang-Tidy 22, three workers, and
the 89 effective lint inputs from 132 compilation-database entries. The command
completed in 255.646 seconds of wall time. Clang-Tidy attributed 430.4903
seconds of aggregate CPU time and 516.8632 seconds of aggregate wall time to
individual checks; those aggregate figures overlap because three translation
units were analyzed concurrently.

Seven checks accounted for 418.1269 seconds, or 97.1%, of attributed check CPU
time:

1. `bugprone-reserved-identifier`: 186.8599 seconds.
2. `cppcoreguidelines-macro-to-enum`: 74.7062 seconds.
3. `modernize-deprecated-headers`: 41.7837 seconds.
4. `cert-dcl37-c`: 34.8574 seconds.
5. `readability-identifier-naming`: 32.5325 seconds.
6. `cert-dcl51-cpp`: 31.7978 seconds.
7. `misc-no-recursion`: 15.5894 seconds.

This is measurement evidence, not approval to remove any check. In particular,
alias names and checks overlapping compiler diagnostics require positive and
negative coverage probes before changing the configured authority.

The initial coverage-preserving curation disables `cert-dcl37-c`,
`cert-dcl51-cpp`, and `bugprone-reserved-identifier`. The two CERT names
instantiate the same reserved-identifier check as the Bugprone name. An isolated
LLVM 22 probe confirmed that each name diagnoses the same cases independently
and that Memly's existing `-Weverything` compilation catches those cases through
`-Wreserved-identifier`. The compiler is therefore the single authority without
duplicating an implied warning flag.

The same curation disables the `cppcoreguidelines-macro-to-enum` alias and its
`modernize-macro-to-enum` implementation. `cppcoreguidelines-macro-usage`
remains enabled with an empty allowed-name expression and diagnoses every tested
integral constant macro. This retains the broader macro policy without proposing
an anonymous unscoped enum that `cppcoreguidelines-use-enum-class` subsequently
rejects.

`hicpp-deprecated-headers` is disabled as an alias while
`modernize-deprecated-headers` remains the authority. The compiler does not
diagnose the tested deprecated C compatibility header, so removing the canonical
check would reduce coverage.

Reprofile the full gate before treating the removed check rows as end-to-end
time saved. Alias profiling attributes shared matcher work unevenly, and
compiler-front-end work remains outside the per-check table.

The next coverage-preserving curation replaces `readability-identifier-naming`
with three main-file YAML checks for Memly's actual naming policy: `m_` fields,
`s_` static variables, and `Enum` type suffixes. A probe containing valid and
invalid forms produced the same five diagnostics under the built-in check and
the replacements. The custom unnamed namespace check now starts at each
main-file anonymous namespace and iterates its direct named children; a probe
covering variables, functions, records, enums, aliases, templates, and nested
namespaces preserved the complete rule.

`readability-function-size` and its `hicpp-function-size` alias are disabled.
Memly configured none of their thresholds, leaving only the built-in
800-statement default while paying their whole-AST cost. `misc-no-recursion`
remains enabled because direct and indirect recursion are intentionally
forbidden. `modernize-deprecated-headers` remains the authority for deprecated C
compatibility headers. `clang-diagnostic-*` is removed from the Clang-Tidy
selection because the mandatory build has already compiled every target with
Memly's warning policy before lint starts; clangd continues to surface the same
compiler diagnostics interactively.

The 2026-08-21 four-worker profile immediately before this second curation
analyzed 89 inputs from 132 compilation-database entries in 129.059 seconds.
Clang-Tidy attributed 287.2500 aggregate CPU seconds to checks. The leading five
were `readability-identifier-naming` at 164.1002 seconds,
`custom-memly-unnamed-namespace-declaration-prefix` at 49.4338,
`modernize-deprecated-headers` at 33.7879, `readability-function-size` at
16.5853, and `misc-no-recursion` at 14.9653. Together they represented 97.1% of
attributed check CPU. That older report did not record a configuration hash, so
it is directional rather than a fully reproducible A/B baseline.

The first fully identified four-worker profile after the second curation used
source snapshot
`a2348f2fa945385020fd9446dc904e627c3aeaf99b9717c2aefc56fe03876b53`,
configuration
`71a6788900768cc145a45030392304199c8592c022b6821795b6d58ba64177b2`, and
compilation database
`84afea053afc2f38d36461f3e40f95df57ad484b6356bf876e3549f9112b142e`. It analyzed
the same 89 inputs from 132 entries in 154.466 seconds and attributed 305.8535
aggregate CPU seconds to checks. The removed identifier-naming and both
function-size names were absent. The three replacement naming checks consumed
0.0521 seconds in aggregate, while the refined unnamed-namespace check consumed
0.0019 seconds. The complete wall time did not improve in that run: retained
`misc-no-recursion`, `modernize-use-using`, `modernize-deprecated-headers`, and
`custom-memly-scalar-parameter-by-value` together consumed 295.6054 seconds of
attributed CPU. This confirms the intended matcher costs were removed, but not
an end-to-end speedup under the observed machine workload.

The 2026-08-21 two-worker profile analyzed the same 89 inputs in 323.071
seconds, compared with 255.646 seconds for the three-worker baseline. Its
per-check table contained 438 rows for 594 enabled check names. Of the 156
enabled names without rows, 128 were `clang-analyzer-*` checks; static-analyzer
execution is outside the AST match-finder profiler. Other absent names can be
preprocessor-only or inactive for the analyzed language and platform.

The report rounds each percentage to one decimal place. In the two-worker
profile, 427 of 438 rows displayed `0.0%`, but each row retained its measured
seconds. Only eleven rows rounded to at least `0.1%`, and the leading seven
accounted for 97.2% of attributed user-plus-system time. A `0.0%` or absent row
does not prove that disabling a check saves no end-to-end time: the profiler
does not attribute compiler-front-end startup, BMI loading, static-analyzer
execution, or preprocessor-only work to those rows.

The next coverage-preserving curation replaces the two retained Modernize
outliers. A main-file custom AST check rejects every Memly-authored `typedef`
while ignoring declarations originating in imported third-party headers. It does
not reject `using` type aliases or code that names or instantiates an alias; the
replaced check never governed those uses. A linear repository verifier checks a
selected authored `.cpp` or `.mpp` for the complete deprecated and redundant C
compatibility-header set. CMake assigns each eligible file its own mandatory
incremental build stamp, so a source edit reruns the verifier only for that
file. The verifier rejects macro-expanded and line-spliced include operands
rather than losing the preprocessor callback's coverage of those forms.

A subsequent experiment compared three otherwise identical configurations. All
five profiles used source snapshot
`a2348f2fa945385020fd9446dc904e627c3aeaf99b9717c2aefc56fe03876b53` and
compilation database
`3966100d041f13ec2ef1050af1a8d47c910bfe7687b667e1e09eb991124167b5`:

| Configuration                                                      | Recursion analysis | Workers | Elapsed seconds |
| ------------------------------------------------------------------ | ------------------ | ------: | --------------: |
| `3a9febccd60e020af52e097e47fb32ddc51d054fa55dab96660fc1fd66b83df7` | enabled            |       4 |         92.0709 |
| `e92a1f13fdbd349688f3952b60d94edb58fc58784cbef9ee3710e1300827fce2` | enabled            |       3 |         126.822 |
| `e92a1f13fdbd349688f3952b60d94edb58fc58784cbef9ee3710e1300827fce2` | enabled            |       4 |         114.352 |
| `dc1eb085a39102baf268648a5ce58ae79ab7d41bdb88abab280edd698ed12d2a` | disabled           |       3 |         157.165 |
| `dc1eb085a39102baf268648a5ce58ae79ab7d41bdb88abab280edd698ed12d2a` | disabled           |       4 |          178.23 |

Direct Terminal measurements are authoritative for comparisons on this machine;
Codex-spawned process durations are excluded because those processes run at a
lower effective priority. In both measured worker-count pairs, recursion-enabled
gates were faster. The four-worker disabled run also accumulated 121.6652
seconds of system time compared with 31.3297 seconds for the preceding enabled
run, so the effect is too large and too repeatable to dismiss as ordinary timing
noise. Per-file completion totals show the same broad effect. At four workers,
disabled recursion increased aggregate implementation-unit duration from 323.0
to 543.9 seconds and module-interface duration from 106.0 to 161.2 seconds. At
three workers, the corresponding ratios were only 1.28 and 1.16. The nonlinear
penalty at higher concurrency further implicates a memory threshold rather than
a local matcher cost.

LLVM 22 implements `misc-no-recursion` as one translation-unit matcher whose
callback constructs a complete call graph and walks its strongly connected
components. The LLVM 22 matcher driver invokes declaration match callbacks
before recursively traversing the declaration's children. The recursion check
therefore performs a real eager call-graph prepass at the translation-unit root,
before the ordinary matcher traversal. A paired warm probe of `DeckService.cpp`
found no isolated translation-unit speedup: recursion analysis added
approximately 94 MiB of maximum resident memory, read approximately 12,000 more
statements, and completed within 0.3 seconds of the disabled runs. Each isolated
process used approximately 4 GiB on this 16 GiB machine.

The best current explanation is therefore a concurrency effect rather than
cheaper per-process analysis. The eager prepass can deserialize or touch BMI and
AST data and, because its cost varies by translation unit, stagger workers
before the shared matcher-heavy phase. Either effect can reduce simultaneous
peak memory, compression, or swapping. The existence and placement of the
prepass are confirmed; its responsibility for the speedup remains an inference.
The discriminating experiment is a randomized enabled/disabled comparison at
one, two, three, and four workers: the effect should disappear at one worker and
grow near the machine's memory limit if phase staggering is responsible.

Recursion analysis remains enabled. The active scalar-parameter enforcement also
covers pointer and member-pointer references in addition to arithmetic and enum
references; reproducing the historical 92-second configuration does not justify
rolling back that coverage.

## Curation Order

1. Record the uncurated aggregate profile and complete command wall time.
2. Investigate the highest attributed time sink, including aliases and any
   equivalent compiler diagnostic, before changing its configured authority.
3. Apply one coverage-preserving change, then rerun the same profile and full
   mandatory build before proceeding to the next time sink.
4. Revisit broad families and the unprofiled static-analyzer or preprocessor
   work only after the attributed top sinks no longer dominate wall time.

Check profiling attributes matcher work rather than compiler-front-end startup,
BMI loading, or all process overhead. Use the reported command wall time for the
complete cost and the per-check report to choose what to investigate.

## Concurrency Baseline

On the 16 GiB macOS development machine, six concurrent workers caused severe
memory-pressure slowdown in the uncurated check set. Deep translation units that
took roughly 15 seconds in an isolated warm probe took approximately 153 through
160 seconds each while six deep workers overlapped. Two workers also regressed
the complete profiled gate by 67.425 seconds relative to three. The project now
defaults to four workers while accepting any positive integer through
`MEMLY_CLANG_TIDY_JOBS`; no project ceiling prevents controlled comparisons on
other machines.

Do not infer throughput from worker count alone. Compare the complete gate wall
time at each process count because resident BMI closures can make additional
workers slower rather than faster.

Complete three-worker mandatory gates on 2026-08-20 took 206, 264, 282, and 335
seconds under different preceding builds and concurrent machine workloads. A
subsequent no-change build ran no Clang-Tidy process and reported a sub-second
lint phase. Touching one Invalidation implementation rebuilt that component and
reran only its six lint inputs, completing the lint phase in less than one
second. Reconfiguring CMake without changing the effective compile commands ran
only the content-stable database comparison and also retained the sub-second
zero-analysis gate.
