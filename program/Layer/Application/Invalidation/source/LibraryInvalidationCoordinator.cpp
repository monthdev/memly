// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Memly/Invalidation/LibraryInvalidationCoordinator.hpp"

#include <qdatetime.h>
#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include <algorithm>
#include <cstdint>
#include <optional>

#include "Memly/Exception/ExceptionBoundary.hpp"
#include "Memly/Invalidation/LibraryInvalidationChannel.hpp"
#include "Memly/Invalidation/LibraryInvalidationTarget.hpp"
#include "Memly/Repository/LibraryRepository.hpp"
#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Application::Invalidation {

LibraryInvalidationCoordinator::LibraryInvalidationCoordinator(
    LibraryInvalidationChannel& LibraryInvalidationChannel,
    Infrastructure::Repository::LibraryRepository& LibraryRepository,
    QObject* Parent)
    : QObject{ Parent }
    , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_LibraryInvalidationChannel{ LibraryInvalidationChannel }
    , m_LibraryRepository{ LibraryRepository }
    , m_LibraryInvalidationQTimer{} {
    this->m_LibraryInvalidationQTimer.setSingleShot(true);
    QObject::connect(&this->m_LibraryInvalidationQTimer, &QTimer::timeout, this, &LibraryInvalidationCoordinator::HandleScheduledInvalidation);
    this->ScheduleNextLibraryInvalidation();
}

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
    Support::Exception::TryCatchWrapper([&]() -> void {
        this->m_LibraryInvalidationQTimer.stop();
        const std::optional<std::int64_t> NextLibraryInvalidationAtMillisecondsSinceEpochOptional{
            this->m_LibraryRepository.ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(
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
