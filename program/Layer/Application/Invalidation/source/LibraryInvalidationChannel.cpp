module Memly.Invalidation;

// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Memly/Invalidation/LibraryInvalidationChannel.hpp"

#include <qdatetime.h>
#include <qobject.h>

#include <cstdint>

namespace Memly::Invalidation {

LibraryInvalidationChannel::LibraryInvalidationChannel(QObject* Parent)
    : QObject{ Parent }
    , m_CurrentSnapshotAsOfMillisecondsSinceEpoch{ static_cast<std::int64_t>(QDateTime::currentMSecsSinceEpoch()) } {
}

[[nodiscard]] auto LibraryInvalidationChannel::GetCurrentSnapshotAsOfMillisecondsSinceEpoch() const noexcept -> std::int64_t {
    return std::int64_t{ this->m_CurrentSnapshotAsOfMillisecondsSinceEpoch };
}

void LibraryInvalidationChannel::SetCurrentSnapshotAsOfMillisecondsSinceEpoch(
    const std::int64_t CurrentSnapshotAsOfMillisecondsSinceEpoch) noexcept {
    this->m_CurrentSnapshotAsOfMillisecondsSinceEpoch = CurrentSnapshotAsOfMillisecondsSinceEpoch;
}

void LibraryInvalidationChannel::PublishInvalidation(const LibraryInvalidationTargetBitset& LibraryInvalidationTargetBitset) noexcept {
    emit this->InvalidationSignal(LibraryInvalidationTargetBitset);
}

}
#endif
