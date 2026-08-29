# Memly Coding Guide

## Maintaining This Guide

Follow [`META_GUIDE.md`](META_GUIDE.md) for the shared structure and routing
requirements of every guide.

Keep this guide scoped to stable rules for Memly project code files and tools
whose direct purpose is enforcing those code rules. Put CMake-file rules and the
maintenance of tools that CMake invokes, configures, or uses to validate
generated build state in
[`CMAKE_MAINTENANCE_GUIDE.md`](CMAKE_MAINTENANCE_GUIDE.md). Put investigation
history, measurements, enforcement coverage changes and gaps, and other patch
evidence in [`PATCH_ARTIFACTS_GUIDE.md`](PATCH_ARTIFACTS_GUIDE.md). Put optional
local hook orchestration in
[`HOOKS_RECOMMENDATION_GUIDE.md`](recommendation/HOOKS_RECOMMENDATION_GUIDE.md),
and put editor diagnostic integration in
[`LINTING_SETUP_RECOMMENDATION_GUIDE.md`](recommendation/LINTING_SETUP_RECOMMENDATION_GUIDE.md).

Update this guide only for a Memly-specific rule that requires agent judgment or
has dedicated enforcement. State a manual rule normatively. Cite each applicable
`custom-memly-*` check by name and each applicable verifier under `tool/` by its
repository-relative path. Never name or enumerate a built-in Clang-Tidy check,
check family, or compiler warning flag
(`tool/agents_documentation/verify_coding_guide_document.py`). Do not duplicate
project source, implementation inventories, current control flow, tool versions,
or formatter, editor, and CI behavior. Prune superseded prose with the rule or
enforcement change.

## Runtime Composition and Services

Keep `ApplicationRuntime` dependencies before dependents in construction and
ownership order, and destroy the QML engine before objects it can access. Expose
only narrow View-layer factories to QML; never expose `ApplicationRuntime` or
let a lower layer retrieve dependencies from it.

Services expose domain capabilities to application commands rather than model
stateful entities. Keep a state-free domain operation on its service rather than
making it file-private solely because it does not use instance state.

## Construction and Initialization

Every class and struct declares an ordinary constructor
(`custom-memly-declared-ordinary-constructor`). Reserve `struct` for data
records and declare its non-deleted constructors `noexcept`
(`custom-memly-data-struct-constructor-noexcept`).

Define a non-template, non-`constexpr`, non-`consteval`, non-deleted constructor
with implementation behavior in the corresponding `.cpp`, whether it belongs to
a class or struct
(`custom-memly-runtime-constructor-definition-in-implementation-file`). Define a
constructor template, class-template constructor, and `constexpr` or `consteval`
constructor in-class
(`custom-memly-interface-required-constructor-definition-in-class`). Define an
ordinary default constructor as `= default` on its first in-class declaration
when the type has no direct fields and every base is intentionally
default-constructed. Define a deleted constructor on its first in-class
declaration.

Every non-deleted, non-defaulted constructor initializes every direct base,
field, and applicable virtual base
(`custom-memly-complete-constructor-initializer-list`) with braces
(`custom-memly-braced-constructor-initializer`). Do not use default member
initializers (`custom-memly-no-default-member-initializer`) or delegation
(`custom-memly-no-delegating-constructor`). Do not default a default constructor
for a type with direct fields
(`custom-memly-no-direct-field-defaulted-default-constructor`).

Brace-initialize named variables and lambda init-captures
(`custom-memly-braced-variable-initialization`). Do not name a local solely to
pass it once (`custom-memly-no-two-statement-pass-through-variable`).

Explicitly spell the type of braced function arguments, assignments, returns,
and default arguments (`custom-memly-explicit-contextual-braced-expression`,
`custom-memly-explicit-contextual-braced-default-argument`). A non-void value
return whose type is neither a pointer nor reference constructs its declared
type (`custom-memly-explicit-return-type-construction`). A forwarding function
template may directly return
`std::invoke(std::forward<LambdaType>(Lambda), ...)` to preserve `void`,
reference, and value results.

Declare all constructors `explicit` (`custom-memly-always-explicit-constructor`)
and no conversion operators (`custom-memly-conversion-operator`).

## Special Members and Declaration Layout

Every class and struct declares the Rule of Five
(`custom-memly-explicit-rule-of-five`). Default or delete each copy and move
operation (`custom-memly-defaulted-or-deleted-copy-move-operation`). Define a
destructor in-class as `= default` or `= delete`, or out of line as `= default`
(`custom-memly-defaulted-or-deleted-destructor-definition`).

After ordinary constructors, group the copy constructor with the copy
assignment, then group the move constructor with the move assignment, then place
the destructor. Use no blank line within either copy or move group and exactly
one blank line between those groups. Separate all other consecutive method
declarations and definitions with exactly one blank line.

Spell `noexcept` on a non-throwing defaulted move and `override` on an
overriding destructor. Do not add `constexpr` to a defaulted special member
solely because it is eligible. Give each operation the narrowest current
semantics; guaranteed copy elision does not require move construction.

A static-only type explicitly deletes its ordinary default constructor.

Place class-level Qt metadata first, then friends. Spell every base's access. A
type with private nested types or fields starts with `private:`; place field
types first, fields in construction order, and another nested type immediately
before its first declaration-site use. Put an implementation-only nested type
last. Constructors follow fields under their intended access; other declarations
follow constructors.

Module interfaces define only eligible defaulted ordinary constructors,
templates, `constexpr` or `consteval` constructors, deleted ordinary
constructors, and defaulted or deleted special members. Define every other
ordinary non-template method and free function in its corresponding `.cpp`
implementation unit (`custom-memly-no-module-interface-function-definition`).

Qualify current-object fields and methods with `this->`, including in
constructor-initializer expressions and `this`-capturing lambdas. Leave
member-initializer identifiers unqualified; qualify static members with their
declaring type.

Leave declaration parameters unnamed
(`custom-memly-no-named-declaration-parameter`). Name a definition parameter
only when referenced. Leave an unconditionally unused parameter unnamed; reserve
`[[maybe_unused]]` for a named parameter whose use is conditionally compiled.

## Parameters, Ownership, Strings, and Results

Pass scalar parameters by value (`custom-memly-scalar-parameter-by-value`). Pass
small non-owning values by value, including `std::source_location`
(`custom-memly-source-location-parameter-by-value`). Pass a non-trivial owning
value by `const T&` to read, `T&` to mutate, and `T&&` to consume; do not copy
it into a by-value parameter.

Use `const std::string&` for an existing owning string and `std::string_view` by
value for general sized text
(`custom-memly-owning-string-parameter-by-reference`,
`custom-memly-string-view-parameter-by-value`). Pass `std::initializer_list` and
`std::span` by value (`custom-memly-non-owning-range-view-parameter-by-value`).
Add top-level `const` in the `string_view` parameter definition
(`custom-memly-string-view-definition-parameter-const`). Reserve
`const char* const` for an already pointer-shaped, null-terminated boundary or
an explicit non-allocating path. Return owner-backed borrowed text as
`std::string_view` by value rather than exposing its owning `std::string` by
reference. Restrict such an accessor to lvalue owners, delete its rvalue
overload, and mark the lvalue declaration `[[clang::lifetimebound]]`. Store
`string_view` only where design enforces its owner and invalidation boundary.

Mark every intentional returned or stored lifetime dependency
`[[clang::lifetimebound]]` on its module-interface declaration after enforcing
the ownership, construction, destruction-order, or full-expression boundary in
the API. The annotation communicates that boundary to cross-unit analysis; it
does not replace the boundary or justify moving a runtime definition into a
module interface.

Declare every non-deleted, non-void method and free function except `main`
`[[nodiscard]]` (`custom-memly-nodiscard-callable`), as well as every enum
(`custom-memly-nodiscard-enum`) and record returned by value
(`custom-memly-nodiscard-returned-record`).

Return `std::expected<SuccessType, ErrorType>` for a recoverable failure and
`std::expected<void, ErrorType>` when success carries no value.
`std::optional<T>` represents absence within a successful result and never an
error channel.

## Error Boundaries

Assert internal preconditions, postconditions, invariants, invalid internal enum
values, and impossible states; do not throw for a programming error. Do not
assert an external or runtime condition that can fail during correct execution;
throw if that failure is neither recoverable here nor represented by a typed
result.

Compile an assertion-only declaration, definition, and call together under
`#if not defined(NDEBUG)` when the API has no Release meaning. Do not suppress a
diagnostic caused solely by leaving that debug-only API active in Release; use
`[[maybe_unused]]` only at a shared Debug/Release boundary whose parameter use
is conditionally compiled.

Outside language-required syntax, only zero and positive one remain unnamed.
Name negative integers (`custom-memly-no-negative-magic-integer`), assertion
integers (`custom-memly-no-magic-integer-in-assertion`), integers in a
const-qualified variable or field's type
(`custom-memly-no-magic-integer-in-constant-declaration-type`). Name every
floating-point literal, including zero
(`custom-memly-no-zero-floating-point-magic-number`).

Write exception messages as a noun phrase followed by a verb phrase, such as
`Database query failed`, rather than verb-first prose.

When Memly originates an unrecoverable runtime failure, throw
`Memly::Exception::MemlyException`
(`custom-memly-explicit-memly-exception-throw`). Use its default source location
unless an adapter preserves an earlier public boundary. Do not add a throw-only
or constructor-forwarding helper.

Keep the non-allocating exception payload separate from boundary logging and
termination. Validate untrusted input before making it internal state;
downstream code may assert the resulting invariant. Do not assert and
runtime-check one invariant unless the checks protect distinct boundaries.

## Database and Persistence Boundaries

Assume only Memly modifies its database; file tampering is unsupported. Values
read may rely on invariants established by Memly writes. Treat their violation
as a programming error and an engine failure as a runtime error at the boundary.

Do not add an engine-named folder or namespace. Group repository reads and
mutations by domain, including in any future Application-owned persistence port.

Keep private SQL with its consumer. Name a single-operation resource and
accessor for its primary `Select`, `Update`, `Insert`, or `Delete`; keep domain
language in repository methods. Separate bootstrap from ordered `MNN_`
migrations without a redundant `Statement` directory.

Commit a migration and its log write in one transaction. Inspect each DuckDB
statement or result once with the nearest standard guard; do not rewrap its
`MemlyException`.

Every repository prepared statement returns rows: reads return decodable
records, and mutations return the affected domain IDs needed by synchronization.
Complete each repository execution in one full-expression chain through
`DecodedTo<RecordType>()` and `AssertRowCount(...)`. Do not name or store a
chain proxy (`custom-memly-no-database-chain-proxy-variable`,
`custom-memly-no-database-chain-proxy-field`). Bootstrap statements are outside
the domain-synchronization contract; inspect their engine errors at the raw
database boundary without requiring returned domain rows.

A decoded row publicly inherits
`Database::DecodableQueryResultRowMixin<ColumnType...>`. Use
`std::optional<ColumnType>` for a nullable column; align the SQL projection,
type pack, and row constructor. Do not materialize a `duckdb::Value` per cell.
Copy string bytes into owning storage, verify physical types once per chunk, and
confine unsafe access to that adapter.

Keep decoding separate from explicit row-count validation; a bounded mismatch is
a programming error.

## Qt and QML Boundaries

Use standard types at non-Qt boundaries. Confine Qt types to Qt-bound code or
required framework, QML, resource, signal, timer, event-loop, and override
interfaces; convert at the boundary.

Expose bridge state and metadata through `Q_PROPERTY`, stable pointer identities
as `CONSTANT`, and bridge commands through `Q_INVOKABLE`.

In a global module fragment, include Qt public physical headers without a module
prefix, such as `<qobject.h>`. Do not include forwarding headers such as
`<QObject>` or umbrella headers such as `<QtCore>`.

## Containers and Non-Owning Access

Use read-only container accessors and `.at()` for non-mutating runtime indices;
use `operator[]` to mutate. Non-throwing access may use `operator[]` after an
assertion establishes bounds, with suppression only on that expression.

## Naming

Do not declare global or namespace-scope variables
(`custom-memly-no-namespace-variable`). The `g_` prefix is forbidden for every
Memly-controlled identifier (`custom-memly-no-global-prefix`).

Prefix non-static data members with `m_`
(`custom-memly-non-static-data-member-prefix`). Prefix static data members and
function-local variables with static or thread storage duration with `s_`
(`custom-memly-static-variable-prefix`). End enum type names in `Enum`
(`custom-memly-enum-type-suffix`).

When representation matters, end a local, definition parameter, field, or
private helper name in the complete unqualified type name. Add only a narrower
semantic prefix; use the type name alone when it fully describes the value.

Named `std::vector`, `std::array`, `std::map`, `std::unordered_map`, `std::set`,
and `std::unordered_set` values use the complete container suffix
(`custom-memly-standard-container-value-suffix`). `std::string` and
`std::string_view` add no suffix unless a callable emphasizes conversion.

Named `std::expected`, `std::variant`, and `std::optional` values use their type
suffixes (`custom-memly-expected-value-suffix`,
`custom-memly-variant-value-suffix`, `custom-memly-optional-value-suffix`).
Begin a lookup with `Try` when empty means not found. State an associative
lookup relationship before its container suffix.

End a type template parameter name in `Type`
(`custom-memly-type-template-parameter-name`).

A wrapper's value parameter repeats the wrapper name, and its field repeats that
name after `m_`. Name a representation getter for its concrete return type; use
a semantic getter only for a domain property.

## Namespaces and File-Private Code

A Memly-owned definition's namespace mirrors its owning component. Architectural
folders and `module` add no namespace component. A target-private module stored
below `module/Internal/` adds `Internal` after the component namespace; lower
organizational folders add nothing. Keep namespace-scope declarations in those
files in that `Internal` namespace and reserve it for that location
(`custom-memly-internal-declaration-namespace`,
`custom-memly-internal-namespace-reserved`). Nest unnamed helpers inside the
matching `Memly` namespace (`custom-memly-unnamed-namespace-nesting`).

Spell each interface's exact named module as its component namespace path with
`::` replaced by `.`, followed by the interface file stem. For example,
`DatabaseRuntime.cppm` in `Memly::Database` declares
`Memly.Database.DatabaseRuntime`; `_MigrationSql.cppm` below `module/Internal/`
declares `Memly.Database.Internal.MigrationSql`. Prefix every owner-only module
interface and corresponding implementation filename with `_`; the physical
marker does not enter the named module. Do not carry other physical
architectural or organizational folders into the module name
(`memly_get_module_interface_metadata()`,
`memly_validate_module_implementation()`).

Every declaration made directly in an unnamed namespace uses `u_`; members of an
unnamed-namespace type retain ordinary member names
(`custom-memly-unnamed-namespace-declaration-prefix`). Reserve `u_` for that
scope (`custom-memly-unnamed-namespace-prefix-reserved`).

Declare every class, struct, union, enum, typedef, and type alias in a `.cppm`,
including target-private and PImpl definitions
(`custom-memly-no-type-declaration-in-implementation-file`). Place each unnamed
block before its earliest consumer; split blocks when first consumers differ.

Use the shortest unambiguous namespace qualification, omitting current and
enclosing components and starting siblings at their nearest common namespace. Do
not introduce a using or alias only to shorten it.

## Lambdas, Templates, and Type Spelling

Do not use direct or indirect recursion.

Do not name a lambda (`custom-memly-no-named-lambda`); pass or invoke it
directly. Put reusable logic in a `u_` helper, or a private method when it needs
private object state.

Invoke indirect callables with `std::invoke` (`custom-memly-use-std-invoke`).
Direct free, static, and member functions and namespace-level library callable
APIs remain direct. A receiver invokes a passed callable with `std::invoke`.

Constrain a type template parameter when its required operations or shape can be
stated. Keep a single-use constraint inline and name a reused semantic concept;
leave a parameter unconstrained only when any substitutable type is intentional.

Use `auto` only when a named variable's type is unnameable or deduction is
required (`custom-memly-no-deduced-variable-type`). Keep the exception local. Do
not use class template argument deduction for a named variable
(`custom-memly-no-class-template-argument-deduction`).

Do not use using declarations or directives
(`custom-memly-no-using-declaration`). Alias only an associated type that a
language or interface form cannot express directly; declare it with `using`,
never `typedef` (`custom-memly-no-typedef`). Spell `typename` on every dependent
qualified type.

## Preprocessor Spelling

Use `#if defined(Macro)` and `#if not defined(Macro)`, not `#ifdef` and
`#ifndef`.

Name an included header literally on one physical directive; do not hide it
behind a macro or line splice. Include C standard-library facilities through
their C++ headers; do not include deprecated or redundant C compatibility
headers (`memly_verify_header_includes()`,
`tool/cmake/verify_header_includes.py`).

## Source Organization

Collocate each externally importable module interface and its corresponding
implementation unit directly under `module/`. Store owner-only module pairs and
target-private inputs under `module/Internal/`; SQL belongs below
`module/Internal/Sql/`. Use only `.cppm` for Memly-authored module interfaces
and `.cpp` for implementation units. Do not add textual C++ headers, C,
Objective-C, Objective-C++, or alternate C++ source extensions under `source/`
or `test/` (`memly_get_module_interface_metadata()`,
`memly_validate_module_implementation()`, `memly_verify_source_extensions()`).

Place physical third-party and standard-library includes in the global module
fragment, then declare the module before imports. Import Memly dependencies; do
not textually include Memly source. An implementation unit belongs to the exact
module declared by its corresponding interface and explicitly imports every
other Memly module it uses.

Give every active module unit one exact named-module declaration:
`export module` in `.cppm`, and `module` in `.cpp`. Use a global module fragment
exactly when the unit has active physical includes, all of which precede that
declaration. Put exact non-reexporting Memly imports in one block immediately
afterward; do not self-import, repeat imports, or use module partitions. Sort
imports first by dot-separated component count and then lexicographically. Keep
imports of one component count adjacent, and separate consecutive component
counts with exactly one blank line. Provably inactive branches do not establish
module structure (`memly_validate_module_unit_structure()`).

## Enforcement and Suppressions

Keep C++ AST enforcement in `.clang-tidy` YAML, including custom queries. Use a
dedicated repository verifier only for a non-AST source or documentation policy
and cite it from the rule it enforces. Do not add a compiled Clang-Tidy
extension or a general prose-style linter.

Keep an unavoidable `NOLINT` suppression local to the exact declaration or
expression and name the suppressed check.
