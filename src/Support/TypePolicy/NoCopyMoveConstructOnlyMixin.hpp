#pragma once

namespace Support::TypePolicy {
class NoCopyMoveConstructOnlyMixin {
protected:
    NoCopyMoveConstructOnlyMixin() noexcept = default;

public:
    NoCopyMoveConstructOnlyMixin(const NoCopyMoveConstructOnlyMixin&) = delete;
    NoCopyMoveConstructOnlyMixin(NoCopyMoveConstructOnlyMixin&&) noexcept = default;
    NoCopyMoveConstructOnlyMixin& operator=(const NoCopyMoveConstructOnlyMixin&) = delete;
    NoCopyMoveConstructOnlyMixin& operator=(NoCopyMoveConstructOnlyMixin&&) = delete;
};
}
