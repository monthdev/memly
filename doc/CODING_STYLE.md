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

This is so the class API and override surface are visible at the declaration
site.
