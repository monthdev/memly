# Memly Coding Style

## Constructor

### Parameter and Member Ordering

Constructor parameter and member ordering of `RuntimeContext`-owned objects must
match the ordering found in `RuntimeContext`.

See \ref RuntimeContext.hpp "RuntimeContext.hpp" and \ref RuntimeContext.cpp
"RuntimeContext.cpp".

### Member Construction

Constructor member construction must use brace-initialized construction in the
initializer list. Class members must not be initialized anywhere else but the
initializer list.

## Header Declaration Surface

Constructors and destructors must always be declared and implemented in the
header even when the implementation is `= default`.

Overrides must also be declared and implemented in the header.

## Forward Declarations

Use forward declarations where the header only needs an incomplete type.

## Copy And Move Surface

Copy and move constructor and assignment operator must explicitly be addressed
for all declared classes and for all declared data-time object structs (DTOs).

## Auto

`auto` is disallowed except where the language or API requires it such as
lambdas, APIs that expose private types, or otherwise unnameable types.

Local lambdas and DuckDB query result iterators are examples of required `auto`
usage.

## Namespace

Source-owned namespaces must match the folder nesting under `src/`.

Anonymous helper namespaces must be nested inside the matching source-owned
namespace.

## Lambdas And Closures

Lambdas and closures must always declare their return type.

## Using

`using` declarations and aliases are disallowed.
