#pragma once

#include <array>
#include <concepts>
#include <cstdint>
#include <utility>

namespace Application::Invalidation {

enum class [[nodiscard]] LibraryInvalidationTargetEnum : std::uint8_t {
    DeckForestSnapshot,
    TargetCount
};

class LibraryInvalidationTargetBitset final {
private:
    std::array<bool, std::to_underlying(LibraryInvalidationTargetEnum::TargetCount)> m_TargetArray;

public:
    template <typename... LibraryInvalidationTargetType>
        requires(sizeof...(LibraryInvalidationTargetType) > 0 and (std::same_as<LibraryInvalidationTargetType, LibraryInvalidationTargetEnum> and ...))
    explicit LibraryInvalidationTargetBitset(const LibraryInvalidationTargetType... LibraryInvalidationTargets) noexcept
        : m_TargetArray{} {
        (Set(LibraryInvalidationTargets), ...);
    }

    explicit LibraryInvalidationTargetBitset(const LibraryInvalidationTargetBitset&) = delete;
    explicit LibraryInvalidationTargetBitset(LibraryInvalidationTargetBitset&&) noexcept = delete;
    auto operator=(const LibraryInvalidationTargetBitset&) -> LibraryInvalidationTargetBitset& = delete;
    auto operator=(LibraryInvalidationTargetBitset&&) -> LibraryInvalidationTargetBitset& = delete;

    [[nodiscard]] auto Contains(LibraryInvalidationTargetEnum) const noexcept -> bool;

private:
    void Set(LibraryInvalidationTargetEnum) noexcept;
};

}
