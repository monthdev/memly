#pragma once

#include <array>
#include <concepts>
#include <cstdint>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Application::Invalidation {

enum class [[nodiscard]] LibraryInvalidationTargetEnum : std::uint8_t {
    DeckForestSnapshot,
    TargetCount
};

class LibraryInvalidationTargetBitset final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    std::array<bool, std::to_underlying(LibraryInvalidationTargetEnum::TargetCount)> m_TargetArray;

public:
    template <typename... LibraryInvalidationTargetType>
        requires(sizeof...(LibraryInvalidationTargetType) > 0 and (std::same_as<LibraryInvalidationTargetType, LibraryInvalidationTargetEnum> and ...))
    explicit LibraryInvalidationTargetBitset(const LibraryInvalidationTargetType... LibraryInvalidationTargets) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_TargetArray{} {
        (Set(LibraryInvalidationTargets), ...);
    }

    [[nodiscard]] auto Contains(LibraryInvalidationTargetEnum) const noexcept -> bool;

private:
    void Set(LibraryInvalidationTargetEnum) noexcept;
};

}
