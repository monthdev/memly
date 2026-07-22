# Memly Agent Coding Guide

## Runtime Composition and Services

Constructor parameters and data members of `RuntimeContext`-owned objects must
follow their order in `RuntimeContext`. See \ref RuntimeContext.hpp
"RuntimeContext.hpp" and \ref RuntimeContext.cpp "RuntimeContext.cpp".

Services form the domain capability surface used by application commands; they
do not model stateful domain entities. A state-free operation remains on its
service when it belongs to that domain surface instead of becoming a
file-private helper merely because it does not use instance state.

## Initialization and Construction

Every Memly type must explicitly declare an ordinary constructor
(`custom-memly-declared-ordinary-constructor`). Constructor definitions must
remain in headers, including `= default` and `= delete` definitions
(`custom-memly-constructor-definition-in-header`).

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

Every constructor, including copy and move constructors, must be `explicit`
(`custom-memly-always-explicit-constructor`). Conversion operators are
disallowed (`custom-memly-conversion-operator`).

## Special-Member Policy

Every Memly class and data-carrying struct must obtain a policy from
`Support::SpecialMemberPolicy` (`custom-memly-required-special-member-policy`):

- `NoCopyNoMoveMixin` for runtime objects with stable identity or state.
- `NoCopyMoveConstructOnlyMixin` for transferable values that may be move
  constructed but cannot be copied or assigned.
- `NonInstantiableMixin` for static-only types.

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

When a type has private nested types or data members, its first access block
must be `private:`. Nested types needed by data-member declarations come first,
then data members in construction order. This data-member-first rule applies to
structs as well as classes.

Constructors immediately follow the data-member block under their intended
access. All remaining declarations follow the constructors.

Headers may contain bodies only for constructors and templates. Every ordinary
non-template method and free function, including a one-line accessor, is
declared in its header and defined in the corresponding source file
(`custom-memly-no-header-function-definition`). Defaulted and deleted
special-member declarations remain in headers because they have no function
body.

Function declaration parameters are unnamed
(`custom-memly-no-named-declaration-parameter`); parameters are named at the
definition.

## Parameters, Ownership, and Strings

Pass scalars and small non-owning value types by value. Pass non-trivial owning
values by reference, and use an rvalue reference when the callee consumes
ownership. Do not copy non-trivial owning values into parameters by value.

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

Do not assert conditions that can fail during correct execution because of
external or runtime state. Throw when such a failure cannot be recovered at the
current boundary and is not represented by a typed recoverable result.

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

Positional DuckDB result decoding may narrowly suppress both
`cppcoreguidelines-avoid-magic-numbers` and `readability-magic-numbers` with a
paired `NOLINTBEGIN` and `NOLINTEND` around the decoding expression.

## Qt and QML Boundaries

Presentation and View own UI-facing Qt and QML integration. Focused Application
runtime-coordination modules may use Qt when their behavior depends on the Qt
event loop, signals, or timers; keep those dependencies contained and out of
domain, service, store, and database contracts. `main.cpp` owns process startup
and QML engine bootstrap. `Support/Runtime/QtApp` contains Qt adapters for
application runtime support such as standard paths and embedded resources.

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

Use container or user-defined type suffixes for non-public-API names when the
suffix materially clarifies representation. Relevant standard-container suffixes
include `Vector`, `Array`, `Map`, `UnorderedMap`, `Set`, and `UnorderedSet`.
`std::string` and `std::string_view` do not add type suffixes except when a
callable name emphasizes conversion to that representation.

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

## Namespaces and File-Private Code

Namespaces for code under `src/` mirror its folder nesting. An unnamed helper
namespace is nested inside that matching namespace. The custom matcher enforces
the minimum structural requirement that it have a Memly namespace ancestor
(`custom-memly-unnamed-namespace-nesting`). Namespace-scope helper functions use
an `a_` prefix (`custom-memly-unnamed-namespace-helper-prefix`).

## Lambdas, Callables, and Type Spelling

Lambdas always declare their return type. Named lambda closure variables are
disallowed (`custom-memly-no-named-lambda`); pass a lambda directly to its
consumer or invoke it immediately. Reusable callable logic belongs in an
unnamed-namespace `a_` helper, or in a private method when it needs class state
or private types.

Invoke stored callables, callable template parameters, function pointers, and
member-function pointers with `std::invoke`.

Deduced variable types through `auto` are disallowed unless the type is
unnameable or the language or API requires deduction. Keep each exception narrow
and explicit; DuckDB query-result iterators are the current example
(`custom-memly-no-deduced-variable-type`).

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
