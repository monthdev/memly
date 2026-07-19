#pragma once

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <utility>

namespace Application::Invalidation {

enum class LibraryInvalidationTargetEnum : std::size_t {
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
    explicit LibraryInvalidationTargetBitset(LibraryInvalidationTargetBitset&&) noexcept = default;
    auto operator=(const LibraryInvalidationTargetBitset&) -> LibraryInvalidationTargetBitset& = delete;
    auto operator=(LibraryInvalidationTargetBitset&&) -> LibraryInvalidationTargetBitset& = delete;

    [[nodiscard]] auto Contains(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) const noexcept -> bool {
        assert(std::to_underlying(LibraryInvalidationTarget) < m_TargetArray.size());
        return m_TargetArray[std::to_underlying(LibraryInvalidationTarget)];
    }

private:
    void Set(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) noexcept {
        assert(std::to_underlying(LibraryInvalidationTarget) < m_TargetArray.size());
        m_TargetArray[std::to_underlying(LibraryInvalidationTarget)] = true;
    }
};

}
