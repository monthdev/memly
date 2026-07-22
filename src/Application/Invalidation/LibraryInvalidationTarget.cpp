#include "Application/Invalidation/LibraryInvalidationTarget.hpp"

#include <cassert>
#include <utility>

namespace Application::Invalidation {

[[nodiscard]] auto LibraryInvalidationTargetBitset::Contains(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) const noexcept -> bool {
    assert(std::to_underlying(LibraryInvalidationTarget) < m_TargetArray.size());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    return m_TargetArray[std::to_underlying(LibraryInvalidationTarget)];
}

void LibraryInvalidationTargetBitset::Set(const LibraryInvalidationTargetEnum LibraryInvalidationTarget) noexcept {
    assert(std::to_underlying(LibraryInvalidationTarget) < m_TargetArray.size());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
    m_TargetArray[std::to_underlying(LibraryInvalidationTarget)] = true;
}

}
