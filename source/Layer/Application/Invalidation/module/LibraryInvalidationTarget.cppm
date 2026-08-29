module;

#include <concepts>
#include <cstdint>

export module Memly.Invalidation.LibraryInvalidationTarget;

export namespace Memly::Invalidation {

enum class [[nodiscard]] LibraryInvalidationTargetEnum : std::uint8_t {
    DeckForestSnapshot = std::uint8_t{ 1 } << std::uint8_t{ 0 },
    TargetEnd = std::uint8_t{ 1 } << std::uint8_t{ 1 },
};

class LibraryInvalidationTargetBitset final {
private:
    std::uint8_t m_TargetBitMask;

public:
    template <typename... LibraryInvalidationTargetType>
        requires(
            sizeof...(LibraryInvalidationTargetType) > 0 and
            (std::same_as<
                 LibraryInvalidationTargetType,
                 LibraryInvalidationTargetEnum> and
                ...)
        )
    explicit LibraryInvalidationTargetBitset(
        const LibraryInvalidationTargetType... LibraryInvalidationTargets
    ) noexcept : m_TargetBitMask{} {
        (this->Set(LibraryInvalidationTargets), ...);
    }

    explicit LibraryInvalidationTargetBitset(
        const LibraryInvalidationTargetBitset&
    ) = delete;
    LibraryInvalidationTargetBitset&
    operator=(const LibraryInvalidationTargetBitset&) = delete;

    explicit LibraryInvalidationTargetBitset(
        LibraryInvalidationTargetBitset&&
    ) = delete;
    LibraryInvalidationTargetBitset&
    operator=(LibraryInvalidationTargetBitset&&) = delete;

    ~LibraryInvalidationTargetBitset() noexcept = default;

    [[nodiscard]] bool Contains(LibraryInvalidationTargetEnum) const noexcept;

private:
    void Set(LibraryInvalidationTargetEnum) noexcept;
};

}
