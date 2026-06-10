# Memly Coding Style

## Constructor Parameter Ordering

Constructor parameter ordering of `RuntimeContext`-owned objects should match
the ordering found in `RuntimeContext`.

This is to enforce consistent class composition ordering.

See \ref RuntimeContext.hpp "RuntimeContext.hpp" and \ref RuntimeContext.cpp
"RuntimeContext.cpp".

## Header Declaration Surface

Constructors and destructors must always be declared and implemented in the
header even when the implementation is `= default`.

Overrides must also be declared and implemented in the header.

Copy and move constructor and assignment operator must explicitly be addressed
for all declared classes and all declared data-time object structs (DTOs).

This is so the class composition, behavior, and override surface are visible at
the declaration site.
