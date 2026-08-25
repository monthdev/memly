module;

#include <cassert>
#include <utility>

#if not defined(NDEBUG)
#include <bit>
#endif

module Memly.Invalidation.LibraryInvalidationTarget;

namespace Memly::Invalidation {

[[nodiscard]] auto LibraryInvalidationTargetBitset::Contains(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) const noexcept -> bool {
    assert(std::has_single_bit(std::to_underlying(LibraryInvalidationTarget)) and
           std::to_underlying(LibraryInvalidationTarget) < std::to_underlying(LibraryInvalidationTargetEnum::TargetEnd));
    return bool{ static_cast<bool>(this->m_TargetBitMask bitand std::to_underlying(LibraryInvalidationTarget)) };
}

void LibraryInvalidationTargetBitset::Set(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) noexcept {
    assert(std::has_single_bit(std::to_underlying(LibraryInvalidationTarget)) and
           std::to_underlying(LibraryInvalidationTarget) < std::to_underlying(LibraryInvalidationTargetEnum::TargetEnd));
    this->m_TargetBitMask or_eq std::to_underlying(LibraryInvalidationTarget);
}

}
