// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Layer/Application/Invalidation/LibraryInvalidationCoordinator.hpp"

#include <qdatetime.h>
#include <qtmetamacros.h>

#include <algorithm>
#include <cstdint>
#include <optional>

#include "Layer/Application/Invalidation/LibraryInvalidationChannel.hpp"
#include "Layer/Application/Invalidation/LibraryInvalidationTarget.hpp"
#include "Layer/Infrastructure/Persistence/Store/Library/LibraryClockStore.hpp"
#include "Support/Runtime/Exception/ExceptionBoundary.hpp"

namespace Layer::Application::Invalidation {

void LibraryInvalidationCoordinator::Invalidate(const LibraryInvalidationTargetBitset& SignaledLibraryInvalidationTargetBitset) noexcept {
    emit this->m_LibraryInvalidationChannel.InvalidationSignal(SignaledLibraryInvalidationTargetBitset);
}

void LibraryInvalidationCoordinator::InvalidateWithReschedule(const LibraryInvalidationTargetBitset& SignaledLibraryInvalidationTargetBitset) noexcept {
    emit this->m_LibraryInvalidationChannel.InvalidationSignal(SignaledLibraryInvalidationTargetBitset);
    this->ScheduleNextLibraryInvalidation();
}

void LibraryInvalidationCoordinator::InvalidateWithRescheduleAndNewSnapshotEpoch(
    const LibraryInvalidationTargetBitset& SignaledLibraryInvalidationTargetBitset) noexcept {
    this->m_LibraryInvalidationChannel.m_CurrentSnapshotAsOfMillisecondsSinceEpoch = static_cast<std::int64_t>(QDateTime::currentMSecsSinceEpoch());
    emit this->m_LibraryInvalidationChannel.InvalidationSignal(SignaledLibraryInvalidationTargetBitset);
    this->ScheduleNextLibraryInvalidation();
}

void LibraryInvalidationCoordinator::HandleScheduledInvalidation() noexcept {
    this->InvalidateWithRescheduleAndNewSnapshotEpoch(LibraryInvalidationTargetBitset{ LibraryInvalidationTargetEnum::DeckForestSnapshot });
}

void LibraryInvalidationCoordinator::ScheduleNextLibraryInvalidation() noexcept {
    Support::Runtime::Exception::TryCatchWrapper([&]() -> void {
        this->m_LibraryInvalidationQTimer.stop();
        const std::optional<std::int64_t> NextLibraryInvalidationAtMillisecondsSinceEpochOptional{
            this->m_LibraryClockStore.ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(
                this->m_LibraryInvalidationChannel.m_CurrentSnapshotAsOfMillisecondsSinceEpoch)
        };
        if (not NextLibraryInvalidationAtMillisecondsSinceEpochOptional.has_value()) {
            return;
        }
        const std::int64_t LibraryInvalidationDelayMilliseconds{ std::max<std::int64_t>(
            0, NextLibraryInvalidationAtMillisecondsSinceEpochOptional.value() - static_cast<std::int64_t>(QDateTime::currentMSecsSinceEpoch())) };
        this->m_LibraryInvalidationQTimer.start(static_cast<int>(LibraryInvalidationDelayMilliseconds));
    });
}

}
#endif
