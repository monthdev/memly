#include "Application/Invalidation/LibraryInvalidationCoordinator.hpp"

#include <QDateTime>
#include <algorithm>
#include <optional>

#include "Infrastructure/Store/Library/LibraryClockStore.hpp"
#include "Runtime/Crash.hpp"

namespace Application::Invalidation {

void LibraryInvalidationCoordinator::Invalidate(const LibraryInvalidationTargetBitset& SignaledLibraryInvalidationTargetBitset) {
    emit m_LibraryInvalidationChannel.InvalidationSignal(SignaledLibraryInvalidationTargetBitset);
}

void LibraryInvalidationCoordinator::InvalidateWithReschedule(const LibraryInvalidationTargetBitset& SignaledLibraryInvalidationTargetBitset) {
    emit m_LibraryInvalidationChannel.InvalidationSignal(SignaledLibraryInvalidationTargetBitset);
    ScheduleNextLibraryInvalidation();
}

void LibraryInvalidationCoordinator::InvalidateWithRescheduleAndCurrentSnapshotEpoch(
    const LibraryInvalidationTargetBitset& SignaledLibraryInvalidationTargetBitset) {
    m_LibraryInvalidationChannel.m_CurrentSnapshotAsOfMillisecondsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
    emit m_LibraryInvalidationChannel.InvalidationSignal(SignaledLibraryInvalidationTargetBitset);
    ScheduleNextLibraryInvalidation();
}

void LibraryInvalidationCoordinator::HandleScheduledInvalidation() {
    InvalidateWithRescheduleAndCurrentSnapshotEpoch(LibraryInvalidationTargetEnum::DeckTreeSnapshot);
}

void LibraryInvalidationCoordinator::ScheduleNextLibraryInvalidation() {
    Runtime::TryCatchWrapper([&]() -> void {
        m_LibraryInvalidationQTimer.stop();
        const std::optional<qint64> NextLibraryInvalidationAtMillisecondsSinceEpoch{ m_LibraryClockStore.ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(
            m_LibraryInvalidationChannel.m_CurrentSnapshotAsOfMillisecondsSinceEpoch) };
        if (not NextLibraryInvalidationAtMillisecondsSinceEpoch.has_value()) {
            return;
        }
        const qint64 LibraryInvalidationDelayMilliseconds{ std::max<qint64>(
            0, NextLibraryInvalidationAtMillisecondsSinceEpoch.value() - QDateTime::currentMSecsSinceEpoch()) };
        m_LibraryInvalidationQTimer.start(static_cast<int>(LibraryInvalidationDelayMilliseconds));
    });
}

}
