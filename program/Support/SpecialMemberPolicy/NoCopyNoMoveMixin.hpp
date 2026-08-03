#pragma once

namespace Support::SpecialMemberPolicy {
class NoCopyNoMoveMixin {
protected:
    explicit NoCopyNoMoveMixin() noexcept = default;
    ~NoCopyNoMoveMixin() noexcept = default;

public:
    explicit NoCopyNoMoveMixin(const NoCopyNoMoveMixin&) = delete;
    explicit NoCopyNoMoveMixin(NoCopyNoMoveMixin&&) = delete;
    auto operator=(const NoCopyNoMoveMixin&) -> NoCopyNoMoveMixin& = delete;
    auto operator=(NoCopyNoMoveMixin&&) -> NoCopyNoMoveMixin& = delete;
};
}
