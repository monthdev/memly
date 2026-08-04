# Memly Agent Coding Guide

## Runtime Composition and Services

Constructor parameters and data members of objects owned by
`CompositionRoot::RuntimeContext` must follow their order in
`RuntimeContext.hpp/cpp`.

Services form the domain capability surface used by application commands; they
do not model stateful domain entities. A state-free operation remains on its
service when it belongs to that domain surface instead of becoming a
file-private helper merely because it does not use instance state.

## Initialization and Construction

Every Memly type must explicitly declare an ordinary constructor
(`custom-memly-declared-ordinary-constructor`). Define every constructor inside
its class or struct declaration wherever that declaration resides, including
`= default` and `= delete` definitions
(`custom-memly-constructor-definition-in-class`).

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

## Special-Member Policy

Every Memly class and data-carrying struct must obtain a policy from
`Support::SpecialMemberPolicy` (`custom-memly-required-special-member-policy`):

- `NoCopyNoMoveMixin` when current control paths require neither copying nor
  moving, including runtime objects with stable identity or state.
- `NoCopyMoveConstructOnlyMixin` only when a current control path requires move
  construction but does not require copying or assignment.
- `NonInstantiableMixin` for static-only types.

Always select the most restrictive policy that supports the type's actual
current use. Do not grant copy or move capability for hypothetical future uses;
loosen the policy only when introducing a control path that requires that
operation. Returning a direct prvalue through guaranteed copy elision does not
require move construction.

A type without a policy-bearing Memly base must inherit its policy directly. Any
directly inherited policy mixin must be private
(`custom-memly-private-direct-special-member-policy`). A derived Memly type must
inherit the policy transitively from an existing policy-bearing base. Exactly
one effective policy is allowed: different policies and direct repetition of a
policy already inherited through another base are rejected
(`custom-memly-nonconflicting-special-member-policy`). A same-policy diamond
through two ordinary bases is also disallowed but cannot be detected by the
current YAML matcher.

Policy consumers must not redeclare copy or move constructors or copy or move
assignment operators (`custom-memly-no-consumer-special-member`), and must not
declare destructors (`custom-memly-no-consumer-destructor`). Each policy mixin
must explicitly default a protected destructor
(`custom-memly-policy-mixin-destructor`).

A `NonInstantiableMixin` consumer must explicitly reiterate its ordinary default
constructor `= delete` as the visual marker that it cannot be instantiated
(`custom-memly-explicit-non-instantiable-default-constructor`). Other consumers
must not delete an ordinary default constructor
(`custom-memly-deleted-default-constructor-only-for-non-instantiable`).

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

Headers may contain bodies only for in-class constructor definitions and
templates. Every ordinary non-template method and free function, including a
one-line accessor, is declared in its header and defined in the corresponding
source file (`custom-memly-no-header-function-definition`). Defaulted and
deleted special-member definitions remain inside their class declarations
because they have no function body.

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

`Layer/Infrastructure/Persistence` groups the database engine boundary, SQL
resources, and concrete repositories. A repository is the domain-shaped
persistence boundary for one domain and groups that domain's reads and
mutations. Do not split read and mutation operations into separate repository
types merely because their result shapes differ. If persistence dependency
inversion is introduced, preserve this domain grouping in the Application-owned
port and make the Infrastructure repository its implementation.

Single-operation SQL resources are classified by their primary SQL statement
under `Select/`, `Update/`, `Insert/`, or `Delete/`. Each resource filename and
accessor begins with that SQL statement name, followed by its domain purpose;
prepared-statement members repeat the same operation name. Repository methods
retain domain-operation language. Each SQL domain exposes one root-level
`<Domain>Sql.hpp/.cpp` accessor pair, and consumers depend on that accessor
rather than on operation folders.

Migration SQL separates unconditional setup under `Bootstrap/`, ordered atomic
migration scripts under `Version/`, and migration-log support statements under
their SQL-operation folders. Versioned migration filenames retain their `MNN_`
execution-order prefix and may contain multiple SQL statement kinds. Seed SQL
uses SQL-operation folders, currently `Insert/`. Do not add a redundant
`Statement/` directory around SQL resources.

`DatabaseRuntime` owns the live DuckDB database and connection and the
repository-facing prepared-statement factory. It retains the single startup
transaction that orders migration before seeding. The ephemeral
`DatabaseMigrator` and `DatabaseSeeder` receive the live connection and own
their respective startup-only direct SQL through raw DuckDB connection,
prepared-statement, and result APIs. The Memly prepared-statement execution and
decoding chain is the capability boundary for database consumers outside the
Database component; internal migration and seeding must not route through it. A
`PreparedStatement` is self-contained after preparation and begins execution
through its own `Execute()` method. `QueryResultDecoder` owns the result from
that execution path and fetches its chunks directly. Do not route an operation
back through `DatabaseRuntime` when the corresponding DuckDB handle already owns
that operation.

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
row-constructor parameter order aligned. The mixin supplies the row's
special-member policy, so the row does not inherit a second policy mixin
directly.

Decoding and row-count validation are separate stages. Express every repository
contract with `QueryResultRowCountRange::ZeroOrMore()`, `Exactly()`,
`AtLeast()`, `AtMost()`, or `Between()`. Bounded row-count mismatches are
programming errors and remain debug assertions.

Positional DuckDB result decoding may narrowly suppress both
`cppcoreguidelines-avoid-magic-numbers` and `readability-magic-numbers` with a
paired `NOLINTBEGIN` and `NOLINTEND` around the decoding expression.

## Qt and QML Boundaries

`Layer/Presentation` and `Layer/View` own UI-facing Qt and QML integration.
Focused `Layer/Application` runtime-coordination modules may use Qt when their
behavior depends on the Qt event loop, signals, or timers; keep those
dependencies contained and out of domain, service, repository, and database
contracts. `main.cpp` owns process startup and QML engine bootstrap.
`Support/Runtime/QtApp` contains Qt adapters for application runtime support
such as standard paths and embedded resources.

Use standard-library types and containers at non-Qt-facing boundaries. Qt types
and containers appear only in explicitly Qt-bound modules or where Qt framework,
QML, resource, signal, timer, event-loop, or override interfaces require them,
with conversion at that boundary.

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

Namespaces for code under `program/` mirror its folder nesting; namespaces for
the four architectural layers therefore begin with `Layer::`. An unnamed helper
namespace is nested inside that matching namespace. The custom matcher enforces
the minimum structural requirement that it have a Memly namespace ancestor
(`custom-memly-unnamed-namespace-nesting`). Every declaration made directly at
unnamed-namespace scope uses an `a_` prefix, including functions, types, enums,
concepts, and aliases; members of an unnamed-namespace type retain their normal
member naming (`custom-memly-unnamed-namespace-declaration-prefix`).

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
belongs in an unnamed-namespace `a_` helper or in a private method when it needs
object state or private types.

Invoke every indirect callable expression with `std::invoke`, including an
immediately invoked lambda, a temporary or returned function object, a stored
callable, a callable template parameter, a function pointer, and a
member-function pointer (`custom-memly-use-std-invoke`). This is to improve
readability over easily missed operator() invocations. Calls outside this
requirement are limited to:

- ordinary direct free or static functions, such as
  `a_ThrowOnIcuError(ErrorCode)`;
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

## Lint Policy

Memly-specific lint enforcement must remain expressible in `.clang-tidy` YAML,
including query-based custom checks. Do not introduce compiled clang-tidy
extensions or a separate text-based style linter solely to enforce this guide.
Rules beyond the YAML interface remain documented conventions.

Keep unavoidable `NOLINT` exceptions local to the exact declaration or
expression and name the suppressed check.
