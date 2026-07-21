# Memly Coding Guide

## Constructor Parameter and Member Ordering

Constructor parameter and member ordering of `RuntimeContext`-owned objects must
match the ordering found in `RuntimeContext`.

See \ref RuntimeContext.hpp "RuntimeContext.hpp" and \ref RuntimeContext.cpp
"RuntimeContext.cpp".

## Constructor Base And Member Construction

Every non-deleted, non-defaulted constructor must explicitly brace-initialize
every direct base class and every non-static data member in its initializer
list. A most-derived constructor must also explicitly brace-initialize every
virtual base class.

Initializer lists must follow the actual initialization order: virtual base
classes first, direct base classes in base-specifier order next, and non-static
data members in declaration order last.

Explicitly defaulted copy and move constructors are exempt because `= default`
cannot be combined with an initializer list and delegates memberwise
construction to the language.

Default member initializers are disallowed. An explicitly defaulted default
constructor may only be used when the class has no direct base classes or
non-static data members.

Delegating constructors are disallowed.

## Variable Initialization

Named variables must use brace initialization.

Lambda init-captures must use brace initialization.

## Header Declaration Surface

Constructors must always be declared and implemented in the header even when the
implementation is `= default`.

Function bodies must only appear in headers for constructors and templates.
Every non-templated non-constructor method and free function must be declared in
the header and implemented in the matching source file, including trivial
one-line accessors.

Explicitly defaulted and deleted special-member declarations remain in headers
because they do not contain function bodies.

Default constructors must not be declared `= delete` outside
`Support::SpecialMemberPolicy::NonInstantiableMixin`. Non-instantiable types
must rely on the inherited policy instead of redeclaring the deleted default
constructor.

Destructors must not be declared.

Destructors are expected to be default behavior because ownership must be
managed by RAII containers and member objects.

Parameters must only be named where methods are implemented, not where methods
are declared.

## Class Declaration Ordering

The body of every class and struct must begin with class-level Qt metadata
macros when applicable followed by all friend declarations.

Classes with private nested types or private data members must use a `private:`
section as their first access section. Nested types required to declare data
members must appear first, followed by the data members in their required
declaration order which determines the construction order of non-static data
members. This data-member-first ordering applies to structs as well as classes.

Constructor declarations must immediately follow the data-member block under
their intended `private:`, `protected:`, or `public:` access. All remaining
class or struct declarations must follow the constructors.

## Parameter Passing

Parameters must be trivial primitive value copies, references, or rvalue
references.

Non-trivial owning values must not be copied into parameters by value.

Use rvalue reference parameters when the callee consumes ownership.

## Attributes, Specifiers, Qualifiers, And Qt Metadata Macros

Classes, data-time object structs, methods, members, parameters, free functions,
and lambdas must use applicable C++ attributes, specifiers, qualifiers,
exception specifications, constraints, and Qt metadata macros.

Class and data-time object struct declarations must use `final` and
`[[nodiscard]]` where they apply.

Every constructor must use `explicit`, including default, multi-parameter, copy,
and move constructors.

Conversion operators are disallowed.

Methods and free functions must use `[[nodiscard]]`, `[[noreturn]]`,
`constexpr`, `consteval`, `inline`, `static`, `virtual`, `override`, `final`,
`const`, lvalue reference qualifiers, rvalue reference qualifiers, `noexcept`,
and trailing return types where they apply.

Templated methods, free functions, and lambdas must use `requires` clauses where
constraints apply.

Members must use `const`, `static`, `inline`, `constexpr`, and `constinit` where
they apply.

Parameters must use `const`, pointer or reference constness, and default
arguments where they apply.

Lambdas must use explicit return types, explicit template parameter lists,
`constexpr`, `consteval`, `static`, `noexcept`, and attributes where they apply.

Qt types and callable surfaces must use `Q_OBJECT`, `Q_GADGET`, `Q_NAMESPACE`,
`Q_PROPERTY`, `Q_SIGNAL`, `Q_SLOT`, `Q_INVOKABLE`, `Q_ENUM`, `Q_FLAG`,
`Q_DECLARE_FLAGS`, `QML_ANONYMOUS`, `QML_ELEMENT`, `QML_NAMED_ELEMENT`,
`QML_SINGLETON`, and `QML_UNCREATABLE` where they apply.

## Programming Errors Versus Runtime Errors

Conditions that can fail only when Memly code is incorrect must be enforced with
debug assertions. This includes violated internal preconditions, postconditions,
invariants, invalid internal enum values, and impossible control states. Do not
throw exceptions for programming errors.

Conditions that can fail during correct execution because of external or runtime
state must not be enforced with assertions. Instead, throw an exception when
such a failure cannot be recovered at the current boundary and is not part of a
typed recoverable result surface.

Validate user input and other untrusted values at their input boundary before
converting them into internal state. Downstream code may then assert the
established invariant instead of repeatedly validating it as a runtime error.

Do not add both an assertion and a runtime error check for the same invariant
unless they enforce distinct boundaries.

## Database Trust Boundary

Memly assumes that its database is modified only through Memly database control
paths. External modification and database-file tampering are outside the
supported runtime contract.

Values read from the database may be assumed to satisfy the schema and the
application invariants established by Memly write paths. Conditions that can
fail only because those invariants were violated by incorrect Memly code (or
external modification of database file) must be enforced with debug assertions
rather than runtime corruption checks.

This assumption does not apply to database engine failures. Those remain runtime
errors at the appropriate database boundary.

## QML Bridge Surface

QML bridge state, object, model, and metadata surfaces must be exposed through
`Q_PROPERTY`.

Stable object and model pointers exposed through `Q_PROPERTY` must use
`CONSTANT` when the pointer identity does not change.

QML bridge application commands must use `Q_INVOKABLE`.

## Data Naming Prefixes

Instance data members must use an `m_` prefix.

Static data members and function-local static variables must use an `s_` prefix.

Global variables are disallowed.

## Type-Suffix Naming

Favor using container/user-defined type suffixes for named variables and
methods/functions besides for public API methods.

Use suffixes such as `Vector`, `Array`, `Map`, `UnorderedMap`, `Set`, and
`UnorderedSet` where they apply.

`std::string` and `std::string_view` is grouped with primitive types and must
not be used as a type suffix to names except for methods/functions where the
emphasis is on the conversion to either of these two types.

Named `std::expected` result variables must use the `Expected` suffix.

Named `std::variant` result variables must use the `Variant` suffix.

Named `std::optional` values must use the `Optional` suffix.

Methods and functions that return `std::optional` must begin with `Try`.

Associative container names must name the lookup relationship before the
container suffix such as `DeckNodeIndexByDeckIdUnorderedMap`.

Enum class type names must end in `Enum`.

## Qt Types And Containers

Application code must use standard library types and containers.

Qt types and containers must only appear at Qt framework, QML bridge, Qt
resource, Qt signal, and method override boundaries where the Qt API requires
them.

Convert between standard library types and Qt types at the boundary.

## Qt Header Includes

Qt includes must use public physical header paths without a module prefix, such
as `<qobject.h>`. Do not use forwarding headers such as `<QObject>` or umbrella
module headers such as `<QtCore>`.

## Qt Dependency Boundary

Presentation and View layers own normal Qt and QML integration.

`main.cpp` may use Qt for process startup and QML engine bootstrap.

Support/Runtime/QtApp may use Qt internally for Qt app runtime support such as
app paths and Qt resources.

Other layers must avoid Qt types in public APIs and must avoid direct Qt
includes in implementation file. Use the Support/Runtime/QtApp boundary instead.

## Container Access

Use read-only container accessors when not modifying container data.

Use `.at()` instead of `operator[]` for non-mutating element access.

Use `operator[]` only for intentional mutation.

## String Types

Read-only string parameters must use `const std::string&` when their inputs are
already `std::string` objects.

Read-only string parameters must use `const std::string_view` when the same API
intentionally accepts either `std::string` objects or string literals or when a
non-allocating control path requires explicit string size information.

String parameters must use `const char* const` when the control path already
receives a character pointer or string literal and introducing an owning string
container would cause a needless heap allocation. Named string constants on such
control paths must also use `const char* const`.

Stored `std::string_view` values must only appear in explicitly non-owning
containers or data types whose owner and invalidation boundary are enforced by
their design.

## Forward Declarations

Use forward declarations where the header only needs an incomplete type.

## Copy And Move Surface

Copy and move constructor and assignment operator must explicitly be addressed
for all declared classes and for all declared data-time object structs.

## Auto

`auto` is disallowed except where the language or API requires it such as
lambdas, APIs that expose private types, or otherwise unnameable types.

Local lambdas and DuckDB query result iterators are examples of required `auto`
usage.

## Namespace

Source-owned namespaces must match the folder nesting under `src/`.

Unnamed helper namespaces must be nested inside the matching source-owned
namespace.

## CMake Source Lists

CMake glob variables must represent exactly one source-tree folder.

Do not use recursive globs for source, header, QML, or SQL file lists.

Split subfolders into their own glob variables instead of adding multiple folder
patterns to one variable.

Order glob declarations and target source lists by source-tree order so the
build file mirrors the repository layout.

## Unnamed Namespace Helpers

Use file-private free helpers in unnamed namespaces for logic that does not use
class state.

File-private helper function names in unnamed namespaces must use an `a_`
prefix.

We want to keep header declaration surfaces very explicit but minimal which also
acts as the testing surface.

## Lambdas

Lambdas must always declare their return type.

## Callable Invocation

Stored callables, callable template parameters, function pointers, and member
function pointers must be invoked with `std::invoke`.

## Using

`using` declarations are disallowed.

`using` aliases are allowed where required by language mechanics such as
template definition type surfaces.

`using` aliases must not be used to shorten ordinary type names.

## Template Typenames

Template typenames must always end in `*Type` and must be named what the
template type is intended to represent.
