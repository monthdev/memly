#pragma once

#include <concepts>
#include <cstdint>

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Application::Invalidation {

enum class [[nodiscard]] LibraryInvalidationTargetEnum : std::uint8_t {
    DeckForestSnapshot = std::uint8_t{ 1 } << std::uint8_t{ 0 },
    TargetEnd = std::uint8_t{ 1 } << std::uint8_t{ 1 }
};

class LibraryInvalidationTargetBitset final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    std::uint8_t m_TargetBitMask;

public:
    template <typename... LibraryInvalidationTargetType>
        requires(sizeof...(LibraryInvalidationTargetType) > 0 and (std::same_as<LibraryInvalidationTargetType, LibraryInvalidationTargetEnum> and ...))
    explicit LibraryInvalidationTargetBitset(const LibraryInvalidationTargetType... LibraryInvalidationTargets) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_TargetBitMask{} {
        (Set(LibraryInvalidationTargets), ...);
    }

    [[nodiscard]] auto Contains(LibraryInvalidationTargetEnum) const noexcept -> bool;

private:
    void Set(LibraryInvalidationTargetEnum) noexcept;
};

}
