# Memly Coding Style

## Constructor Parameter and Member Ordering

Constructor parameter and member ordering of `RuntimeContext`-owned objects must
match the ordering found in `RuntimeContext`.

See \ref RuntimeContext.hpp "RuntimeContext.hpp" and \ref RuntimeContext.cpp
"RuntimeContext.cpp".

## Constructor Member Construction

Constructor member construction must use brace-initialized construction in the
initializer list. Class members must not be initialized anywhere else but the
initializer list.

## Variable Initialization

Named variables must use brace initialization.

Lambda init-captures must use brace initialization.

## Header Declaration Surface

Constructors must always be declared and implemented in the header even when the
implementation is `= default`.

Destructors must not be declared.

Destructors are expected to be default behavior because ownership must be
managed by RAII containers and member objects.

Parameters must only be named where methods are implemented, not where methods
are declared.

## Parameter Passing

Parameters must be trivial primitive value copies, references, or rvalue
references.

Non-trivial owning values must not be copied into parameters by value.

Use rvalue reference parameters when the callee consumes ownership.

Value parameters only apply for `const std::string_view` as described in the
string view rule.

## Attributes, Specifiers, Qualifiers, And Qt Metadata Macros

Classes, data-time object structs, methods, members, parameters, free functions,
and lambdas must use applicable C++ attributes, specifiers, qualifiers,
exception specifications, constraints, and Qt metadata macros.

Class and data-time object struct declarations must use `final` and
`[[nodiscard]]` where they apply.

Constructors and conversion operators must use `explicit` where implicit
conversion is not part of the intended API.

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

## String Views

Read-only string API parameters must use `const std::string_view` instead of
C-style string pointer or array types.

Named string constants must use `std::string_view` when they need a name.

Named `std::string_view` values used at C and Qt string boundaries are expected
to wrap the complete null-terminated string, not a substring. Passing `.data()`
to the boundary is the convention.

Do not name `std::string_view` values if they are immediately being passed as a
parameter just a single time.

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
