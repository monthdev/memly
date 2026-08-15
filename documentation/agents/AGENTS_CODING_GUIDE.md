# Memly Agent Coding Guide

## Runtime Composition and Services

`Memly::CompositionRoot::ApplicationRuntime` owns the live application object
graph by value. Its constructor initializers and data members follow dependency
order from lower-level dependencies to higher-level dependents. `main.cpp`
constructs the Qt application, `ApplicationRuntime`, and the QML engine in that
order so the engine is destroyed before its factories and their dependencies.
Expose only narrow View-layer factories to QML; never expose
`ApplicationRuntime` or allow a lower layer to retrieve dependencies from it.

Services form the domain capability surface used by application commands; they
do not model stateful domain entities. A state-free operation remains on its
service when it belongs to that domain surface instead of becoming a
file-private helper merely because it does not use instance state.

## Initialization and Construction

Every Memly type must explicitly declare an ordinary constructor
(`custom-memly-declared-ordinary-constructor`). A data-carrying record is
declared with `struct`; define every struct constructor inside its declaration
and declare every non-deleted struct constructor `noexcept`
(`custom-memly-data-struct-constructor-noexcept`). For classes, declare every
non-template, non-`constexpr`, non-deleted constructor inside the class
declaration, but define it in the corresponding `.cpp` implementation file,
including when it is explicitly defaulted
(`custom-memly-runtime-constructor-definition-in-implementation-file`). Define a
`constexpr` constructor, constructor template, or constructor of a class
template inside its class declaration so its definition remains reachable where
constant evaluation or template instantiation requires it
(`custom-memly-header-required-constructor-definition-in-class`). A deleted
constructor is the other language-required exception: `= delete` must appear on
its first declaration, so its in-class declaration is also its definition and
cannot be repeated in an implementation file.

Every non-deleted, non-defaulted constructor must explicitly initialize every
direct base, every non-static data member, and every virtual base for which it
is the most-derived constructor
(`custom-memly-complete-constructor-initializer-list`). Each constructor
initializer uses brace initialization
(`custom-memly-braced-constructor-initializer`). Default member initializers are
disallowed (`custom-memly-no-default-member-initializer`), as are delegating
constructors (`custom-memly-no-delegating-constructor`). A default constructor
may use `= default` only when the type has no direct bases or non-static data
members (`custom-memly-no-stateful-defaulted-default-constructor`); explicitly
defaulted copy and move constructors remain allowed.

Named variable definitions and lambda init-captures must use brace
initialization (`custom-memly-braced-variable-initialization`) and direct-list
spelling wherever the language permits it.

Do not introduce a named local solely to pass it once. In particular, a
two-statement block consisting of a local declaration followed by a returned
one-argument call that passes the local must construct the argument directly in
the call (`custom-memly-no-two-statement-pass-through-variable`).

Contextually typed braced initializer lists are disallowed. When a function
argument, return expression, assignment, or default argument uses braced
construction, explicitly spell the constructed type
(`custom-memly-explicit-contextual-braced-expression`,
`custom-memly-explicit-contextual-braced-default-argument`). Every non-void
value return whose declared return type is neither a reference nor a pointer
must explicitly brace-construct that declared type, including when returning an
existing expression such as `std::move(Value)`
(`custom-memly-explicit-return-type-construction`). Named declarations and
constructor initializer lists remain direct-list initialized because their
target type is already stated. A function-template forwarding adapter may
directly return a `std::invoke` expression so one expression preserves `void`,
reference, and value result categories without compile-time branching; the
explicit-return check recognizes this exception.

Every constructor, including copy and move constructors, must be `explicit`
(`custom-memly-always-explicit-constructor`). Conversion operators are
disallowed (`custom-memly-conversion-operator`).

## Special Members

Every Memly class and data-carrying struct explicitly declares the complete Rule
of Five (`custom-memly-explicit-rule-of-five`). Copy and move operations must be
explicitly `= default` or `= delete`
(`custom-memly-defaulted-or-deleted-copy-move-operation`). A destructor must
either be defined in its class declaration as `= default` or `= delete`, or be
declared there and defined out of line as `= default`
(`custom-memly-defaulted-or-deleted-destructor-definition`).

Place the declarations immediately after the ordinary constructors in this exact
grouping, with a blank line between groups:

```cpp
explicit Type(const Type&) = delete;
auto operator=(const Type&) -> Type& = delete;

explicit Type(Type&&) = delete;
auto operator=(Type&&) -> Type& = delete;

~Type() noexcept = default;
```

Add `noexcept` to a defaulted move operation when the operation is non-throwing,
and add `override` to a destructor that overrides a virtual base destructor. Do
not explicitly add `constexpr` to a defaulted special member merely because the
operation is eligible for constant evaluation; omit the qualifier and let the
language determine its implicit `constexpr` status. Use an explicit `constexpr`
constructor only when a concrete constant-evaluation control path requires its
reachable definition. Each special member operation must be individually
defaulted or deleted according to the type's narrowest required semantics. Do
not grant copy or move capability for hypothetical future uses; loosen an
operation only when introducing a control path that requires it. Returning a
direct prvalue through guaranteed copy elision does not require move
construction.

A static-only type explicitly deletes its ordinary default constructor as the
visual marker that it cannot be instantiated. A type may declare its destructor
in the class and define it out of line as `= default`, including when a PImpl
requires the implementation type to be complete at the definition. YAML checks
enforce the presence and permitted definition forms of the five special members.
Their source order and separating blank lines remain a documented formatting
rule because AST matchers do not retain that presentation.

Forward-declare a type in a header when every use there supports that
declaration alone, and include its owning header in the implementation file that
requires the complete definition or enumerators. This applies to Memly and
third-party types, including enums when their opaque declaration is sufficient.

## Declaration Layout and Definition Placement

Class-level Qt metadata macros come first inside a class or struct, followed by
friend declarations.

Every base specifier explicitly states `public`, `protected`, or `private`.
Never rely on the different default inheritance access of `class` and `struct`.

When a type has private nested types or data members, its first access block
must be `private:`. Nested types needed by data-member declarations come first,
then data members in construction order. This data-member-first rule applies to
structs as well as classes. Other nested types appear immediately before the
first declaration that requires them. A nested type used exclusively by
out-of-class implementation bodies appears at the end of the class declaration.

Constructors immediately follow the data-member block under their intended
access. All remaining declarations follow the constructors.

Headers may contain bodies only for data-struct constructors, templates,
`constexpr` constructors, and explicitly defaulted or deleted special members,
including in-class constructor templates and constructors of class templates.
Every ordinary non-template method and free function, including a one-line
accessor, is declared in its header and defined in the corresponding `.cpp`
source file (`custom-memly-no-header-function-definition`). Defaulted class
ordinary constructors are defined out of class in the corresponding source file
unless they are `constexpr`. A destructor may use the out-of-line defaulted form
described under Special Members.

Inside a non-static member function, explicitly qualify every access to the
current object's data members and methods with `this->`. This includes member
accesses in constructor-initializer expressions and lambdas that capture `this`;
a constructor's member-initializer identifiers are not access expressions and
remain unqualified. Qualify static members with their declaring type because
they do not belong to an object instance.

Function declaration parameters are unnamed
(`custom-memly-no-named-declaration-parameter`). A definition parameter is named
only when its body references it. An intentionally unreferenced parameter
remains unnamed instead of receiving a name and `[[maybe_unused]]`.

## Parameters, Ownership, and Strings

Pass scalars and small non-owning value types by value. Pass non-trivial owning
values by reference, and use an rvalue reference when the callee consumes
ownership. Do not copy non-trivial owning values into parameters by value. Pass
`std::source_location` by constant reference.

Use `const std::string&` when a read-only input is already an owning string. Use
`std::string_view` by value when an API intentionally accepts either strings or
literals, or when a non-allocating path needs explicit size information
(`custom-memly-string-view-parameter-by-value`). Add top-level `const` to a
by-value `std::string_view` parameter at its definition
(`custom-memly-string-view-definition-parameter-const`).

Use `const char* const` when the control path already receives a character
pointer or literal and introducing an owning string would allocate needlessly.
Named string constants on that path use `const char* const`, or
`constexpr const char*` when constant evaluation applies.

Stored `std::string_view` values belong only in explicitly non-owning types or
containers whose owner and invalidation boundary are enforced by their design.

## Result and Callable Contracts

Every non-deleted, non-void method and free function other than `main` is
`[[nodiscard]]` (`custom-memly-nodiscard-callable`). Every enum is
`[[nodiscard]]` (`custom-memly-nodiscard-enum`), as is every Memly class or
struct returned by value (`custom-memly-nodiscard-returned-record`). Lambdas are
exempt from the callable-level requirement because they are immediately consumed
under the lambda policy below.

Recoverable operation APIs expose success-oriented results. When a call can
return a recoverable error, it returns `std::expected<SuccessType, ErrorType>`;
use `std::expected<void, ErrorType>` when success carries no value.
`std::optional<T>` represents absence within a successful result and never
serves as an error channel.

## Error and Database Boundaries

Use debug assertions for conditions that can fail only when Memly code is
incorrect: violated internal preconditions, postconditions, invariants, invalid
internal enum values, and impossible control states. Do not throw for a
programming error.

Only zero and positive one may remain as unnamed integer literals. Negative
integer literals must be introduced through a named constant
(`custom-memly-no-negative-magic-integer`). Every floating-point literal must be
introduced through a named constant; zero-valued floating-point literals are
covered by `custom-memly-no-zero-floating-point-magic-number` because the
built-in magic-number check always exempts them. Integer literals in assertions
obey the same policy (`custom-memly-no-magic-integer-in-assertion`). Integer
literals in a const-qualified non-parameter variable or field's type spelling
obey the same policy; this includes direct and nested class template arguments
(`custom-memly-no-magic-integer-in-constant-declaration-type`).

Do not assert conditions that can fail during correct execution because of
external or runtime state. Throw when such a failure cannot be recovered at the
current boundary and is not represented by a typed recoverable result.

Write exception-message prose as a noun phrase followed by a verb phrase, such
as `Database query failed`. Do not use verb-first wording such as
`Failed to execute database query`.

Throw `Memly::Exception::MemlyException` explicitly. Its constructor captures
the throwing call site's source location by default; pass an existing source
location only when an adapter intentionally preserves an earlier public
operation boundary. Do not add forwarding functions whose only behavior is to
throw or forward construction arguments into `MemlyException`.

Keep the non-allocating `MemlyException` payload and the outer
`ExceptionBoundary` logging and termination behavior in separate types and
implementation files within the `Exception` component.

Validate user input and other untrusted values at their input boundary before
converting them into internal state. Downstream code may assert the established
invariant instead of repeating runtime validation. Do not add both an assertion
and a runtime check for one invariant unless they protect distinct boundaries.

Memly assumes its database is modified only through Memly database control
paths. External modification and database-file tampering are outside the
supported runtime contract. Values read from the database may therefore be
assumed to satisfy schema and application invariants established by Memly write
paths. Violations of those invariants are programming errors; database-engine
failures remain runtime errors at the database boundary.

`Layer/Infrastructure/Database` contains the concrete embedded-database
boundary, and `Layer/Infrastructure/Repository` contains its repositories. The
embedded database engine is a fixed implementation detail rather than a
swappable architectural dimension; do not introduce an engine-named folder or
namespace around these components. A repository is the domain-shaped persistence
boundary for one domain and groups that domain's reads and mutations. Do not
split read and mutation operations into separate repository types merely because
their result shapes differ. If persistence dependency inversion is introduced,
preserve this domain grouping in the Application-owned port and make the
Infrastructure repository its implementation.

DuckDB SQL resources are implementation details of their direct consumer.
Repository SQL resources live under
`Layer/Infrastructure/Repository/source/_Sql/<Domain>`, while their
`_<Domain>Sql.hpp/.cpp` accessor pairs live directly under the repository's
`source/_Sql` root. Migration SQL resources and `_MigrationSql.hpp/.cpp`
likewise live under `Layer/Infrastructure/Database/source/_Sql`. `Database` and
`Repository` each form one CMake component with one public `include/` root and
one private `source/` root.

Single-operation SQL resources are classified by their primary SQL statement
under `Select/`, `Update/`, `Insert/`, or `Delete/`. Each resource filename and
accessor begins with that SQL statement name, followed by its domain purpose;
prepared-statement members repeat the same operation name. Repository methods
retain domain-operation language. Each repository SQL domain exposes its own
target-private `_Sql/_<Domain>Sql.hpp/.cpp` accessor pair, and consumers depend
on that pair rather than on operation folders.

Migration SQL separates unconditional setup under `Bootstrap/`, ordered
migration scripts under `Version/`, and migration-log support statements under
their SQL-operation folders. Versioned migration filenames retain their `MNN_`
execution-order prefix and may contain schema and data operations. Do not add a
redundant `Statement/` directory around SQL resources.

Database construction is an rvalue-only ownership chain.
`DatabaseMigrator::ApplyMigrations()` completes migration and migration-log
writes in one transaction, then returns the completed `DatabaseRuntime` by value
to the composition root. Only the completed migration chain releases the
runtime.

`Layer/Infrastructure/Database` is one component and target containing startup
migration, raw transaction execution and rollback handling, the completed live
database and connection, the repository-facing prepared-statement factory,
shared database error guards, and the prepared-statement execution and
result-decoding chain. `Layer/Infrastructure/Repository` is one component and
target containing the domain-grouped repositories and their SQL. Required
initial defaults are inserted by `M02_SeedTableDefaults.sql` after
`M01_CreateSchema.sql`, and future changes to those defaults belong in later
migrations. `DatabaseRuntime` does not expose the bootstrap transaction runner.
A `PreparedStatement` is self-contained after preparation and begins execution
through its own `Execute()` method. Its `QueryResultDecoder` owns the execution
result and fetches chunks directly. Do not route an operation back through
`DatabaseRuntime` when the corresponding DuckDB handle already owns that
operation.

Guard constructed DuckDB prepared statements and query results with
`i_ThrowOnPreparedStatementError()` and `i_ThrowOnQueryResultError()`,
respectively. Chunk-fetch failures expose `duckdb::ErrorData` instead and remain
guarded at the decoding boundary. These guards contain the DuckDB state
inspection; do not wrap their resulting `MemlyException` throw in another
database-error forwarding helper.

Prepared-statement execution and result decoding form one ephemeral, rvalue-only
chain beginning at `PreparedStatement::Execute()`. Invoke `WithParameters()` for
one or more parameters and `WithoutParameters()` for a parameterless statement.
Decode consumed rows with `DecodedTo<QueryResultRowType>()`. Terminate the chain
with `AssertRowCount(QueryResultRowCountRange)`. Use `ZeroOrMore()` when any
result count, including zero, is valid; otherwise supply the bounded range
required by the repository contract. This terminal call is the only operation
that releases decoded records to the repository. Never name or store any
intermediate execution, decoder, or decoded-result proxy
(`custom-memly-no-database-chain-proxy-variable` and
`custom-memly-no-database-chain-proxy-field`). The source-location reference
captured by `PreparedStatement::Execute()` remains valid through that chained
full expression. A statement intentionally returning no consumed rows may
explicitly discard the decoder produced by `WithParameters()` or
`WithoutParameters()`.

A database-decoded row publicly inherits
`Database::DecodableQueryResultRowMixin<ColumnType...>`. The ordered column-type
pack generates compile-time positional decoding and constructs the concrete row
without a repository-owned DuckDB decoder. Use `std::optional<ColumnType>` in
that pack for a nullable result column; SQL `NULL` in a non-optional column
violates a programming invariant. Keep the SQL projection, column-type pack, and
row-constructor parameter order aligned. The metadata mixin does not define the
concrete row's ownership contract; every decoded row declares its own Rule of
Five. Decode each fetched `DataChunk` through its unified vector formats; do not
materialize an intermediate `duckdb::Value` for each cell. Construct an owning
`std::string` directly from DuckDB's `string_t` bytes so each string crosses the
database boundary with one required byte copy. Verify every column's physical
storage type once per fetched chunk, then use `GetDataUnsafe<ColumnType>()` only
while decoding that same verified chunk; do not repeat `GetData<ColumnType>()`
for every cell. Confine the required unsafe-buffer suppression to this adapter
because DuckDB exposes the unified format array and its physical column storage
without bounds-carrying views.

Decoding and row-count validation are separate stages. Express every repository
contract with `QueryResultRowCountRange::ZeroOrMore()`, `Exactly()`,
`AtLeast()`, `AtMost()`, or `Between()`. Bounded row-count mismatches are
programming errors and remain debug assertions.

Positional DuckDB result decoding may narrowly suppress both
`cppcoreguidelines-avoid-magic-numbers` and `readability-magic-numbers` with a
paired `NOLINTBEGIN` and `NOLINTEND` around the decoding expression.

## Qt and QML Boundaries

`Layer/Presentation` and `Layer/View` own UI-facing Qt and QML integration.
Focused `Layer/Application` runtime-coordination components may use Qt when
their behavior depends on the Qt event loop, signals, or timers; keep those
dependencies contained and out of domain, service, repository, and database
contracts. `main.cpp` owns process startup and QML engine bootstrap.
`Support/QtApp` contains Qt adapters for application runtime support such as
standard paths and embedded resources.

Use standard-library types and containers at non-Qt-facing boundaries. Qt types
and containers appear only in explicitly Qt-bound components or where Qt
framework, QML, resource, signal, timer, event-loop, or override interfaces
require them, with conversion at that boundary.

TODO: The two paragraphs above will be subject to change as architecture has not
solidified yet.

QML bridge state, objects, models, and metadata are exposed through
`Q_PROPERTY`. Stable object and model pointers use `CONSTANT` when their
identity does not change. QML bridge application commands use `Q_INVOKABLE`.

Qt includes use public physical header paths without a module prefix, such as
`<qobject.h>`. Do not use forwarding headers such as `<QObject>` or umbrella
module headers such as `<QtCore>`.

## Containers and Non-Owning Access

Use read-only container accessors when data is not being modified. Use `.at()`
instead of `operator[]` for non-mutating element access, and use `operator[]`
for intentional mutation.

Non-throwing bounded access may use `operator[]` after an assertion establishes
the internal index invariant. Narrowly suppress
`cppcoreguidelines-pro-bounds-constant-array-index` and
`cppcoreguidelines-pro-bounds-avoid-unchecked-container-access` on that exact
expression.

## Naming

Global and namespace-scope variables are disallowed
(`custom-memly-no-namespace-variable`).

The `g_` prefix is forbidden for every Memly identifier, without exception
(`custom-memly-no-global-prefix`). Global scope is not an ownership category in
Memly: exported namespace-scope declarations use ordinary names, target-private
declarations use `i_`, and translation-unit-local declarations use `u_`.

Use representation suffixes for non-layer-API names when the concrete
representation matters. For every type governed by this rule, regardless of
whether it is defined by Memly, the standard library, or a third-party library,
use its complete unqualified type name as the suffix. Add a semantic prefix only
when it states a narrower role or distinguishes multiple values of that type.
When the type itself fully describes the value, use the type name as the entire
name; for example, `duckdb::ErrorData ErrorData{}`, not `FetchError` or
`FetchErrorData`.

Relevant standard-container suffixes include `Vector`, `Array`, `Map`,
`UnorderedMap`, `Set`, and `UnorderedSet`
(`custom-memly-standard-container-value-suffix`). `std::string` and
`std::string_view` do not add type suffixes except when a callable name
emphasizes conversion to that representation.

Named `std::expected`, `std::variant`, and `std::optional` values end in
`Expected`, `Variant`, and `Optional`, respectively
(`custom-memly-expected-value-suffix`, `custom-memly-variant-value-suffix`,
`custom-memly-optional-value-suffix`). A lookup or probe returning
`std::optional` begins with `Try` when an empty result means that the requested
value was not found.

Associative-container names state the lookup relationship before the container
suffix, such as `DeckNodeIndexByDeckIdUnorderedMap`.

Type template parameters are named for the role they represent. Their names end
in `Type` (`custom-memly-type-template-parameter-name`).

A wrapper type's wrapped-value constructor parameter repeats the complete
wrapper type name, and its wrapped-value member repeats that name after the `m_`
prefix. For example, `MutatedId(std::string&& MutatedId)` stores `m_MutatedId`.
Never shorten either name to a generic representation such as `Identifier` or
`m_Id`.

Getters on wrapper and representation-bearing Memly types normally name the
concrete returned type instead of a generic concept such as `GetValue` or
`GetText`; for example, an accessor exposing an underlying `std::string` is
named `GetStdString()`. Use a semantic getter name when the returned value is a
domain property rather than a wrapper's underlying representation.

## Namespaces and File-Private Code

Every named namespace under `program/` begins with `Memly::`, followed by the
owning target folder and then any meaningful folder hierarchy below that target
root. Public declarations therefore match their `Memly/<Target>/...` include
path. For example, Domain declarations use `Memly::Domain`, Bridge declarations
use `Memly::Bridge`, and declarations directly under the Repository and Database
target roots—including their private SQL accessors—use `Memly::Repository` and
`Memly::Database`. Architectural grouping folders above a target root, such as
`Layer`, `Application`, `Infrastructure`, `Presentation`, `View`, and `Support`,
do not appear in C++ namespace names. An implementation-organization folder
whose name begins with `_` is likewise namespace-transparent: declarations in
`source/_Sql/_DeckSql.hpp/.cpp` remain directly in the owning `Memly::<Target>`
namespace rather than adding `_Sql` or `Sql`. An unnamed helper namespace is
nested inside the complete matching named namespace. The custom matcher enforces
the minimum structural requirement that it have a `Memly` namespace ancestor
(`custom-memly-unnamed-namespace-nesting`).

A namespace-scope type or free function shared by translation units within one
target, but absent from that target's public include surface, uses the `i_`
prefix. The prefix is required in target-private `_*.hpp/.cpp` helper pairs and
reserved exclusively for declarations at that boundary
(`custom-memly-target-private-declaration-prefix` and
`custom-memly-target-private-prefix-reserved`). Do not apply `i_` merely because
a class member is private; instance and static members retain their ordinary
member naming.

Every declaration made directly at unnamed-namespace scope uses a `u_` prefix,
including functions, types, enums, concepts, and aliases; members of an
unnamed-namespace type retain their normal member naming
(`custom-memly-unnamed-namespace-declaration-prefix`). A `u_` declaration is
confined to one translation unit, whereas an `i_` declaration is a
target-private cross-translation-unit interface.

A custom type whose required visibility is confined to one translation unit is
declared in that translation unit's unnamed namespace. Do not place it in a
header, define it inside a function, or add an incomplete nested declaration
solely to scope the type under a class. Move it into a header or class
declaration only when another translation unit or the enclosing type's
declaration must name it.

Place each unnamed namespace block immediately above the first function or
method definition that uses a declaration introduced by that block. Split
declarations among multiple unnamed namespace blocks when their first consumers
differ; do not collect file-private declarations at the top of a translation
unit merely because all reopened blocks denote the same unnamed namespace.

Qualify names with the shortest namespace prefix that resolves unambiguously
from the current scope. Omit current and enclosing namespace components, and
begin sibling-namespace references at their nearest common enclosing namespace.
Do not introduce using-declarations, using-directives, or aliases solely to
shorten qualification. The remaining prefix length intentionally signals how far
the referenced declaration is from the current namespace a.k.a. the dependency
distance; unnecessary full qualification adds visual clutter and obscures that
dependency-distance signal.

## Lambdas, Callables, and Type Spelling

Lambdas always declare their return type. Named lambda closure variables are
disallowed (`custom-memly-no-named-lambda`); pass a lambda directly to its
consumer or invoke it immediately with `std::invoke`. Reusable callable logic
belongs in an unnamed-namespace `u_` helper or in a private method when it needs
object state or private types.

Invoke every indirect callable expression with `std::invoke`, including an
immediately invoked lambda, a temporary or returned function object, a stored
callable, a callable template parameter, a function pointer, and a
member-function pointer (`custom-memly-use-std-invoke`). This is to improve
readability over easily missed operator() invocations. Calls outside this
requirement are limited to:

- ordinary direct free or static functions, such as
  `u_ThrowOnIcuError(IcuErrorCode)`;
- ordinary direct member functions, such as `IcuBreakIterator.next()`; and
- namespace-level library callable APIs designed for direct function syntax,
  such as `std::ranges::copy(...)` and `std::ranges::equal(...)`.

Passing a callable to another function is not an invocation at that call site;
the receiving implementation invokes it with `std::invoke`.

Prefer concept-constrained type template parameters over unconstrained
`typename` or `class` parameters whenever the accepted operations or type shape
can be stated at the template boundary. Keep a constraint inline when it has one
consumer; introduce a named namespace-scope concept when the same semantic
contract is reused. Leave a type template parameter unconstrained only when the
template intentionally accepts any substitutable type.

Deduced variable types through `auto` are disallowed unless the type is
unnameable or the language or API requires deduction. Keep each exception narrow
and explicit; DuckDB query-result iterators are the current example
(`custom-memly-no-deduced-variable-type`).

Class template argument deduction is disallowed for named variable declarations.
Explicitly spell every class template argument
(`custom-memly-no-class-template-argument-deduction`).

Using-declarations and using-directives are disallowed
(`custom-memly-no-using-declaration`). Type aliases are allowed only where
language mechanics require them, not to shorten ordinary type names.

Dependent qualified type names explicitly use `typename`, including type-only
contexts where the language permits omission.

## Preprocessor Spelling

Conditional directives use `#if defined(Macro)` and `#if not defined(Macro)`,
not `#ifdef` and `#ifndef`.

## CMake Source Organization

Each CMake glob variable represents exactly one source-tree folder. Do not use
recursive globs or combine multiple folder patterns in one variable. Split
subfolders into their own variables.

Order glob declarations and target source lists by source-tree order so the
build file mirrors the repository layout.

A component stores public headers under `include/Memly/<Component>/<Header>.hpp`
and implementation files directly in a lowercase `source/` folder. Include every
public Memly header through its full `Memly/<Component>/<Header>.hpp` path. The
`Memly/` prefix prevents collisions with dependencies, and the component folder
identifies the owning include surface without repeating the complete source-tree
path. Private implementation headers remain under `source/` and do not become
part of this public include surface. Private SQL resources and their
target-private accessor pairs remain under the owning component's `source/_Sql`
subtree.

A declaration-and-implementation helper pair whose `.hpp` and `.cpp` both remain
private under one target's `source/` tree begins both filenames with `_`, such
as `_ThrowOnDatabaseError.hpp/.cpp` and `_Sql/_DeckSql.hpp/.cpp`. The prefix
visually identifies target-private includes at their include sites and sorts
these helpers before implementation files corresponding to exposed headers. This
applies to every target, not only the Database component. Namespace-scope types
and free functions declared by such a pair use `i_`; other declarations retain
their ordinary naming. Do not carry a private path marker into C++ namespaces or
declarations: a leading underscore followed by the uppercase first letter of a
PascalCase name is reserved to the implementation.

Each target declares its source globs, target, include roots, resources, and
direct link dependencies in the `CMakeLists.txt` beside its owned source tree.
Do not add a `CMakeLists.txt` whose only responsibility is forwarding through an
architectural directory hierarchy. The nearest list file that owns a target adds
its directly required target directories with `add_subdirectory`; the
repository-root build file retains only project-wide toolchain, dependency,
warning, lint, verification, and final build-gate policy. Register every
component header and translation unit through the shared CMake component helper
so full-tree linting and header-assignment verification remain complete.

Each logical component maps to one CMake target. Header-only components use an
`INTERFACE` target; components with translation units use a static library. Do
not subdivide one logical component into public declaration facades plus a
shared implementation bag. Application `Domain`, `IndexCache`, and `Service` are
each flat components with one public include surface, one implementation folder,
one namespace, and one target. View `Bridge`, Infrastructure `Database` and
`Repository`, and Support `Exception` and `QtApp` likewise each use one
component target. The QML module remains a separate Qt backing target that
consumes Bridge's QML-registration headers and compiled implementation.

Publish only a component interface's `include/` directory and headers. Add a
`source/` directory only as a private include root when implementation helpers
need it. Do not publish `program/` as a broad include root and do not introduce
layer or top-level umbrella targets.

Every `.cpp` includes its same-stem header first. For each header and
implementation file, retain the smallest include set that both clangd's strict
include diagnostics and Clang-Tidy's `misc-include-cleaner` accept. Do not
repeat an include in an implementation file merely because its declarations are
used there when the associated header already supplies those declarations and
both cleaners accept the smaller set. Neither cleaner detects every benign
redundant include, so manually avoid a known superfluous include without
treating silence from the tools as proof of global minimality.

Express required include relationships as CMake target-link relationships. Use
`PUBLIC` when a dependency is required to compile a component's public headers
and `PRIVATE` when it is confined to the source implementation. While public
headers expose third-party types, the corresponding third-party target
necessarily remains a public dependency; defer narrowing that edge until
representation hiding is deliberately introduced.

## Lint Policy

Memly-specific lint enforcement must remain expressible in `.clang-tidy` YAML,
including query-based custom checks. Do not introduce compiled clang-tidy
extensions or a separate text-based style linter solely to enforce this guide.
Rules beyond the YAML interface remain documented conventions.

Run Include What You Use through `memly-include-what-you-use` and Clang-Tidy
through `memly-clang-tidy`, not through compilation-boundary tool properties.
Every Memly translation unit depends on the serialized lint stamps, so a normal
build completes both gates before compiling any changed Memly translation unit.
Each stamp depends on its policy inputs, `compile_commands.json`, and the source
files it analyzes; unchanged inputs do not repeat the analysis. Do not cap
either full-tree tool's worker count.

Before Codex may finish, the project `Stop` integrity hook runs the same C/C++,
CMake, QML, SQL, and repository-wide Prettier formatters, CMake linter, and
pinned tool versions as formatting CI; Doxygen remains an independent workflow.
The hook rewrites only files whose formatted contents differ so unchanged source
timestamps do not invalidate downstream build work. It configures the
`macos-debug-local` preset only when the Ninja build tree is absent or the
selected preset files change, then always invokes the incremental preset build.
Ninja runs the mandatory Include What You Use, Clang-Tidy, header verification,
compilation, and linking stages only when their actual inputs changed. A
formatting, CMake linting, configuration, or build failure returns the turn to
Codex for repair. Every applicable gate must pass. Do not bypass this hook.

The integrity hook invokes standalone formatter and CMake-linter executables
from `PATH` at the same pinned versions used by formatting CI. Do not use `npx`
or otherwise download formatters during hook execution. It validates every
required executable and version before rewriting any file. Formatter and
editor-only tool requirements ordinarily belong to their direct consumers—the
integrity hook and editor configuration—not to CMake configuration. The LLVM
toolchain is the deliberate exception: CMake requires matching LLVM Clang C and
C++ compilers, `clang-tidy`, `run-clang-tidy`, `clangd`, and `clang-format` from
the canonical LLVM installation even when an individual Clang tool is consumed
only by the hook or editor. CMake also requires Include What You Use and its
compilation-database driver built against that same Clang major version.
Non-LLVM formatting tools such as Prettier remain requirements of the hook that
invokes them rather than configuration prerequisites.

ClangFormat owns C, C++, and Objective-C source files; CMakeFormat owns CMake
listfiles; QMLFormat owns QML; and SQL Formatter owns SQL. Prettier owns every
remaining file for which it can infer or has explicitly been assigned a parser.
The root `.prettierignore` excludes the specialized formatter families so a
future Prettier parser cannot create overlapping formatter authority. Both the
CI and integrity hook invoke Prettier with `--ignore-unknown`: CI passes every
tracked file, while the hook additionally passes untracked, non-ignored files so
a newly created configuration file is formatted before it is committed. Assign
an unusual structured configuration filename an explicit parser in
`.prettierrc.json`; do not force an unsupported format through an unrelated
parser merely to claim universal coverage.

VS Code's native format-on-save pipeline owns frequently edited program text:
clangd formats C, C++, and Objective-C; the Qt QML extension and `qmlls` format
QML; and the official SQL Formatter extension formats DuckDB SQL. These
providers return edits through VS Code's awaited save transaction rather than
rewriting an already saved file. Do not duplicate these formatters with Lint
Runner write tools or competing formatter extensions.

Lint Runner remains the editor adapter for CLI diagnostics and for formatting
CMake and Prettier-owned configuration and documentation files. Its write tools
intentionally run after every save of those infrequently edited files, including
automatic and extension-initiated saves. Keep language and project extensions
such as clangd, CMake Tools, and Qt QML for persistent editor services that a
CLI runner cannot provide.

Memly auto-saves after one second of inactivity and retains CMake Tools'
save-before-build behavior so compilation cannot silently consume stale editor
buffers. VS Code deliberately skips native format-on-save participants for an
automatic save; formatting therefore occurs when an explicit Save or Save All
operation saves a dirty document, without interrupting delayed auto-save. A
document already made clean by automatic save remains unformatted until it is
edited and explicitly saved or the repository integrity hook runs. This
manual-only native behavior applies to C, C++, Objective-C, QML, and SQL; Lint
Runner formats its CMake and Prettier-owned files after automatic save. When
CMake Tools saves a dirty buffer before a build, VS Code awaits the native
formatter before the save and build continue. The integrity hook remains the
final formatting authority for every file family.

CMake Tools copies the active configure preset's `compile_commands.json` to the
repository root after configuration. Editor-side clangd, Clang-Tidy, and Include
What You Use all consume that root mirror; do not hard-code an editor tool to
one preset's build directory. CMake's mandatory build-time lint gates continue
to consume the compilation database in their own `${CMAKE_BINARY_DIR}`. A fresh
or clean workspace must complete CMake configuration before editor C++
diagnostics can use the target-specific compilation context.

Keep one root `.clang-tidy` policy. `misc-include-cleaner` and clangd's strict
missing- and unused-include diagnostics govern implementation-file include sets.
Include What You Use governs header include and forward-declaration decisions.
Analyze a header that has a corresponding implementation through that real
implementation translation unit so IWYU observes the completeness requirements
instantiated there, but emit only diagnostics whose location is the associated
header. Analyze a genuinely header-only file through CMake's one-header public
or private probe. Never also run that probe for an associated header: its
reduced context can contradict the implementation-associated result. Do not use
`--check_also` to reanalyze headers through unrelated consumer contexts. Keep
IWYU's normal forward-declaration policy enabled. The root `.iwyu.imp` contains
only narrow provider mappings required to model third-party public headers
accurately.

The mandatory Clang-Tidy gate runs the complete `.clang-tidy` policy once over
implementation translation units and once with every Memly header as a main
file. Main-file header analysis is required because `misc-include-cleaner` does
not evaluate an included header's own include set. Enable CMake's native public-
and private-header verification for every component so each generated probe
includes exactly one header in its owning target's compile context. A normal
build requires the independently callable `memly-header-verification` target
before its final executable is complete. Ordinary component compilation may
proceed after the lint gates without waiting for the complete
header-verification set; only the final executable is gated by that set.
Preserve the system-include classification of imported and explicitly system
public dependencies; Memly include directories remain warning-checked. Declare
public headers under `HEADERS`, target-private headers under `PRIVATE_HEADERS`,
and implementation translation units under `SOURCES` in each component target.
Do not place headers in `SOURCES` and rely on filename-based classification.

Lint Runner invokes Include What You Use only for the active header. The
`tool/run_include_what_you_use.py` adapter maps that header to its
same-component, same-stem implementation translation unit, filters out
implementation-file diagnostics, and falls back to the corresponding
CMake-generated probe only for a header without an implementation. The full
CMake gate uses the same adapter and policy. Keep repository developer-tool
adapters under the root `tool/` directory rather than inside an editor-specific
configuration directory.

Keep unavoidable `NOLINT` exceptions local to the exact declaration or
expression and name the suppressed check.
