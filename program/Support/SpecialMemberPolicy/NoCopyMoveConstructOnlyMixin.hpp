#pragma once

namespace Support::SpecialMemberPolicy {
class NoCopyMoveConstructOnlyMixin {
protected:
    explicit constexpr NoCopyMoveConstructOnlyMixin() noexcept = default;
    ~NoCopyMoveConstructOnlyMixin() noexcept = default;

public:
    explicit NoCopyMoveConstructOnlyMixin(const NoCopyMoveConstructOnlyMixin&) = delete;
    explicit constexpr NoCopyMoveConstructOnlyMixin(NoCopyMoveConstructOnlyMixin&&) noexcept = default;
    auto operator=(const NoCopyMoveConstructOnlyMixin&) -> NoCopyMoveConstructOnlyMixin& = delete;
    auto operator=(NoCopyMoveConstructOnlyMixin&&) -> NoCopyMoveConstructOnlyMixin& = delete;
};
}
