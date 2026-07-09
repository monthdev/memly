#pragma once

namespace Support::TypePolicy {
class NonInstantiableMixin {
protected:
    NonInstantiableMixin() = delete;

public:
    NonInstantiableMixin(const NonInstantiableMixin&) = delete;
    NonInstantiableMixin(NonInstantiableMixin&&) = delete;
    NonInstantiableMixin& operator=(const NonInstantiableMixin&) = delete;
    NonInstantiableMixin& operator=(NonInstantiableMixin&&) = delete;
};
}
