# Memly Coding Style

## Constructor

### Parameter and Member Ordering

Constructor parameter and member ordering of `RuntimeContext`-owned objects must
match the ordering found in `RuntimeContext`.

This is to enforce consistent class composition ordering.

See \ref RuntimeContext.hpp "RuntimeContext.hpp" and \ref RuntimeContext.cpp
"RuntimeContext.cpp".

### Member Construction

Constructor member construction must use brace-initialized construction in the
initializer list. Class members must not be initialized anywhere else but the
initializer list.

This is for easy viewing of the class composition.

## Header Declaration Surface

Constructors and destructors must always be declared and implemented in the
header even when the implementation is `= default`.

Overrides must also be declared and implemented in the header.

This is so the class composition, behavior, and override surface are visible at
the declaration site.

## Copy And Move Surface

Copy and move constructor and assignment operator must explicitly be addressed
for all declared classes and for all declared data-time object structs (DTOs).

This is so ownership and value semantics for a class are intentionally
considered and set instead of simply letting the compiler infer it from
inheritance, special members, or default language rules.
