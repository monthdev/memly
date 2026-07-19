#pragma once

namespace Support::SpecialMemberPolicy {
class NonInstantiableMixin {
public:
    explicit NonInstantiableMixin() = delete;
    explicit NonInstantiableMixin(const NonInstantiableMixin&) = delete;
    explicit NonInstantiableMixin(NonInstantiableMixin&&) = delete;
    auto operator=(const NonInstantiableMixin&) -> NonInstantiableMixin& = delete;
    auto operator=(NonInstantiableMixin&&) -> NonInstantiableMixin& = delete;
};
}
