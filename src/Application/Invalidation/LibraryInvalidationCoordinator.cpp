#include "Application/Invalidation/LibraryInvalidationCoordinator.hpp"

#include <qdatetime.h>
#include <qtmetamacros.h>

#include <algorithm>
#include <cstdint>
#include <optional>

#include "Application/Invalidation/LibraryInvalidationChannel.hpp"
#include "Application/Invalidation/LibraryInvalidationTarget.hpp"
#include "Infrastructure/Store/Library/LibraryClockStore.hpp"
#include "Support/Runtime/ExceptionBoundary.hpp"

namespace Application::Invalidation {

void LibraryInvalidationCoordinator::Invalidate(const LibraryInvalidationTargetBitset& SignaledLibraryInvalidationTargetBitset) noexcept {
    emit m_LibraryInvalidationChannel.InvalidationSignal(SignaledLibraryInvalidationTargetBitset);
}

void LibraryInvalidationCoordinator::InvalidateWithReschedule(const LibraryInvalidationTargetBitset& SignaledLibraryInvalidationTargetBitset) noexcept {
    emit m_LibraryInvalidationChannel.InvalidationSignal(SignaledLibraryInvalidationTargetBitset);
    ScheduleNextLibraryInvalidation();
}

void LibraryInvalidationCoordinator::InvalidateWithRescheduleAndCurrentSnapshotEpoch(
    const LibraryInvalidationTargetBitset& SignaledLibraryInvalidationTargetBitset) noexcept {
    m_LibraryInvalidationChannel.m_CurrentSnapshotAsOfMillisecondsSinceEpoch = static_cast<std::int64_t>(QDateTime::currentMSecsSinceEpoch());
    emit m_LibraryInvalidationChannel.InvalidationSignal(SignaledLibraryInvalidationTargetBitset);
    ScheduleNextLibraryInvalidation();
}

void LibraryInvalidationCoordinator::HandleScheduledInvalidation() noexcept {
    InvalidateWithRescheduleAndCurrentSnapshotEpoch(LibraryInvalidationTargetBitset{ LibraryInvalidationTargetEnum::DeckForestSnapshot });
}

void LibraryInvalidationCoordinator::ScheduleNextLibraryInvalidation() noexcept {
    Support::Runtime::TryCatchWrapper([&]() -> void {
        m_LibraryInvalidationQTimer.stop();
        const std::optional<std::int64_t> NextLibraryInvalidationAtMillisecondsSinceEpochOptional{
            m_LibraryClockStore.ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(m_LibraryInvalidationChannel.m_CurrentSnapshotAsOfMillisecondsSinceEpoch)
        };
        if (not NextLibraryInvalidationAtMillisecondsSinceEpochOptional.has_value()) {
            return;
        }
        const std::int64_t LibraryInvalidationDelayMilliseconds{ std::max<std::int64_t>(
            0, NextLibraryInvalidationAtMillisecondsSinceEpochOptional.value() - static_cast<std::int64_t>(QDateTime::currentMSecsSinceEpoch())) };
        m_LibraryInvalidationQTimer.start(static_cast<int>(LibraryInvalidationDelayMilliseconds));
    });
}

}
