# Memly Agent Coding Guide

## Maintaining This Guide

Update this guide only for a stable, Memly-specific rule that requires agent
judgment or has dedicated enforcement. State a manual rule normatively; cite a
custom `.clang-tidy` check by name and a dedicated enforcement script by path.
Do not duplicate source or configuration, including implementation inventories,
current control flow, tool versions, or formatter, editor, and CI behavior.
Prune superseded prose with the rule or enforcement change.

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
(`custom-memly-header-required-constructor-definition-in-class`). Define an
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

After ordinary constructors, place the copy constructor and assignment together,
then the move constructor and assignment, then the destructor, with a blank line
between groups.

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

Headers define only eligible defaulted ordinary constructors, templates,
`constexpr` or `consteval` constructors, deleted ordinary constructors, and
defaulted or deleted special members. Define every other ordinary non-template
method and free function in its corresponding `.cpp`
(`custom-memly-no-header-function-definition`).

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
an explicit non-allocating path. Store `string_view` only where design enforces
its owner and invalidation boundary.

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

Include Qt public physical headers without a module prefix, such as
`<qobject.h>`. Do not include forwarding headers such as `<QObject>` or umbrella
headers such as `<QtCore>`.

## Containers and Non-Owning Access

Use read-only container accessors and `.at()` for non-mutating runtime indices;
use `operator[]` to mutate. Non-throwing access may use `operator[]` after an
assertion establishes bounds, with suppression only on that expression.

## Naming

Do not declare global or namespace-scope variables
(`custom-memly-no-namespace-variable`). The `g_` prefix is forbidden for every
Memly-controlled identifier (`custom-memly-no-global-prefix`).

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

A Memly-owned public definition's namespace mirrors its path below
`include/Memly`; folders above the target and private `_` folders add no
component. Nest unnamed helpers inside the matching `Memly` namespace
(`custom-memly-unnamed-namespace-nesting`).

A namespace-scope type, enum, alias, or free function in a target-private
`_*.hpp/.cpp` pair uses `i_`, which is reserved for that boundary
(`custom-memly-target-private-declaration-prefix`,
`custom-memly-target-private-prefix-reserved`). Do not use `i_` merely because a
class member is private.

Every declaration made directly in an unnamed namespace uses `u_`; members of an
unnamed-namespace type retain ordinary member names
(`custom-memly-unnamed-namespace-declaration-prefix`). Reserve `u_` for that
scope (`custom-memly-unnamed-namespace-prefix-reserved`).

Declare every class, struct, union, enum, typedef, and type alias in an `.hpp`,
including target-private and PImpl definitions
(`custom-memly-no-type-declaration-in-implementation-file`). Place each unnamed
block before its earliest consumer; split blocks when first consumers differ.

Use the shortest unambiguous namespace qualification, omitting current and
enclosing components and starting siblings at their nearest common namespace. Do
not introduce a using or alias only to shorten it.

## Lambdas, Templates, and Type Spelling

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
language or interface form cannot express directly. Spell `typename` on every
dependent qualified type.

## Preprocessor Spelling

Use `#if defined(Macro)` and `#if not defined(Macro)`, not `#ifdef` and
`#ifndef`.

## Source and CMake Organization

Give each CMake glob variable one folder; do not recurse or combine patterns.
Order globs and target sources by source-tree order.

Store public headers under `include/Memly/<Component>/` and ordinary
implementations under `source/`. Include public headers by complete
`Memly/<Component>/...` paths; keep private headers and SQL under the owning
`source/` tree.

Use only `.hpp` for Memly-authored C++ headers and `.cpp` for translation units.
Do not add C, Objective-C, Objective-C++, module-interface, or alternate C++
source extensions under `program/` or `test/`
(`tool/verify_source_extensions.py`).

Prefix both files in a target-private helper pair with `_`; do not carry that
marker into C++ names.

Declare a target's globs, include roots, resources, and direct dependencies
beside its source tree. Do not add a forwarding-only architectural
`CMakeLists.txt`. Keep repository-wide policy at the root, add target
directories from their nearest owner, and register components through the shared
helper.

Map one ordinary C++ component to one component target: `INTERFACE` when
header-only, otherwise static. Publish only its `include/` surface; add
`source/` privately only for helpers. Do not publish `program/` or add layer
umbrella targets.

A `.cpp` includes its implemented same-stem header first. Keep the smallest
include set accepted by the gates (`tool/run_include_what_you_use.py`); remove
known redundancy even if undiagnosed.

Express include relationships through target links: `PUBLIC` for public headers
and `PRIVATE` for implementation. Keep a third-party dependency public while its
types remain exposed.

Classify files as `HEADERS`, `PRIVATE_HEADERS`, or `SOURCES`; never place a
header in `SOURCES`.

## Enforcement and Suppressions

Keep Memly lint enforcement in `.clang-tidy` YAML, including custom queries. Do
not add a compiled extension or text-style linter solely for this guide; rules
beyond YAML remain conventions.

All applicable configured gates must pass. Keep `.iwyu.imp` limited to narrow
third-party provider mappings.

Keep an unavoidable `NOLINT` suppression local to the exact declaration or
expression and name the suppressed check.
