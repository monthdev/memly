#pragma once

namespace Support::SpecialMemberPolicy {
class NoCopyMoveConstructOnlyMixin {
protected:
    explicit NoCopyMoveConstructOnlyMixin() noexcept = default;
    ~NoCopyMoveConstructOnlyMixin() noexcept = default;

public:
    explicit NoCopyMoveConstructOnlyMixin(const NoCopyMoveConstructOnlyMixin&) = delete;
    explicit NoCopyMoveConstructOnlyMixin(NoCopyMoveConstructOnlyMixin&&) noexcept = default;
    auto operator=(const NoCopyMoveConstructOnlyMixin&) -> NoCopyMoveConstructOnlyMixin& = delete;
    auto operator=(NoCopyMoveConstructOnlyMixin&&) -> NoCopyMoveConstructOnlyMixin& = delete;
};
}
