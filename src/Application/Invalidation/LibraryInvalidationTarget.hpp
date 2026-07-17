#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <utility>

namespace Application::Invalidation {

enum class LibraryInvalidationTargetEnum : std::size_t {
    DeckForestSnapshot,
    TargetCount
};

class LibraryInvalidationTargetBitset final {
public:
    LibraryInvalidationTargetBitset(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) noexcept
        : m_TargetArray{} {
        Set(LibraryInvalidationTarget);
    }

    LibraryInvalidationTargetBitset(const LibraryInvalidationTargetBitset&) = delete;
    LibraryInvalidationTargetBitset(LibraryInvalidationTargetBitset&&) noexcept = default;
    LibraryInvalidationTargetBitset& operator=(const LibraryInvalidationTargetBitset&) = delete;
    LibraryInvalidationTargetBitset& operator=(LibraryInvalidationTargetBitset&&) = delete;

    [[nodiscard]] bool Contains(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) const noexcept {
        assert(std::to_underlying(LibraryInvalidationTarget) < m_TargetArray.size());
        return m_TargetArray[std::to_underlying(LibraryInvalidationTarget)];
    }

private:
    friend LibraryInvalidationTargetBitset operator|(LibraryInvalidationTargetEnum, LibraryInvalidationTargetEnum) noexcept;
    friend LibraryInvalidationTargetBitset operator|(LibraryInvalidationTargetBitset&&, LibraryInvalidationTargetEnum) noexcept;

    void Set(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) noexcept {
        assert(std::to_underlying(LibraryInvalidationTarget) < m_TargetArray.size());
        m_TargetArray[std::to_underlying(LibraryInvalidationTarget)] = true;
    }

    std::array<bool, std::to_underlying(LibraryInvalidationTargetEnum::TargetCount)> m_TargetArray{};
};

[[nodiscard]] inline LibraryInvalidationTargetBitset operator|(const LibraryInvalidationTargetEnum LeftLibraryInvalidationTarget,
                                                               const LibraryInvalidationTargetEnum RightLibraryInvalidationTarget) noexcept {
    LibraryInvalidationTargetBitset LibraryInvalidationTargetBitset{ LeftLibraryInvalidationTarget };
    LibraryInvalidationTargetBitset.Set(RightLibraryInvalidationTarget);
    return LibraryInvalidationTargetBitset;
}

[[nodiscard]] inline LibraryInvalidationTargetBitset operator|(LibraryInvalidationTargetBitset&& LibraryInvalidationTargetBitset,
                                                               const LibraryInvalidationTargetEnum LibraryInvalidationTarget) noexcept {
    LibraryInvalidationTargetBitset.Set(LibraryInvalidationTarget);
    return std::move(LibraryInvalidationTargetBitset);
}

}
