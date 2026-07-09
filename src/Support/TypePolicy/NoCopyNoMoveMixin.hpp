#pragma once

namespace Support::TypePolicy {
class NoCopyNoMoveMixin {
protected:
    NoCopyNoMoveMixin() noexcept = default;

public:
    NoCopyNoMoveMixin(const NoCopyNoMoveMixin&) = delete;
    NoCopyNoMoveMixin(NoCopyNoMoveMixin&&) = delete;
    NoCopyNoMoveMixin& operator=(const NoCopyNoMoveMixin&) = delete;
    NoCopyNoMoveMixin& operator=(NoCopyNoMoveMixin&&) = delete;
};
}
