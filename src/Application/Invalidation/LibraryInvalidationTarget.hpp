#pragma once

#include <bitset>
#include <cstddef>
#include <utility>

namespace Application::Invalidation {

enum class LibraryInvalidationTargetEnum : std::size_t {
    DeckTreeSnapshot,
    TargetCount
};

class LibraryInvalidationTargetBitset final {
public:
    LibraryInvalidationTargetBitset(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) noexcept {
        Set(LibraryInvalidationTarget);
    }

    ~LibraryInvalidationTargetBitset() = default;

    [[nodiscard]] bool Contains(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) const noexcept {
        return m_TargetBitset[std::to_underlying(LibraryInvalidationTarget)];
    }

private:
    friend LibraryInvalidationTargetBitset operator|(LibraryInvalidationTargetEnum, LibraryInvalidationTargetEnum) noexcept;
    friend LibraryInvalidationTargetBitset operator|(LibraryInvalidationTargetBitset, LibraryInvalidationTargetEnum) noexcept;

    void Set(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) noexcept {
        m_TargetBitset.set(std::to_underlying(LibraryInvalidationTarget));
    }

    std::bitset<std::to_underlying(LibraryInvalidationTargetEnum::TargetCount)> m_TargetBitset{};
};

[[nodiscard]] inline LibraryInvalidationTargetBitset operator|(const LibraryInvalidationTargetEnum LeftLibraryInvalidationTarget,
                                                               const LibraryInvalidationTargetEnum RightLibraryInvalidationTarget) noexcept {
    LibraryInvalidationTargetBitset LibraryInvalidationTargetBitset{ LeftLibraryInvalidationTarget };
    LibraryInvalidationTargetBitset.Set(RightLibraryInvalidationTarget);
    return LibraryInvalidationTargetBitset;
}

[[nodiscard]] inline LibraryInvalidationTargetBitset operator|(LibraryInvalidationTargetBitset LibraryInvalidationTargetBitset,
                                                               const LibraryInvalidationTargetEnum LibraryInvalidationTarget) noexcept {
    LibraryInvalidationTargetBitset.Set(LibraryInvalidationTarget);
    return LibraryInvalidationTargetBitset;
}

}
