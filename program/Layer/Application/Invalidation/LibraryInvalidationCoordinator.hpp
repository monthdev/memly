// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "Layer/Application/Invalidation/LibraryInvalidationTarget.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Persistence::Store::Library {
class LibraryClockStore;
}

namespace Layer::Application::Invalidation {

class LibraryInvalidationChannel;

class LibraryInvalidationCoordinator final : public QObject, private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    Q_OBJECT
private:
    LibraryInvalidationChannel& m_LibraryInvalidationChannel;
    Infrastructure::Persistence::Store::Library::LibraryClockStore& m_LibraryClockStore;
    QTimer m_LibraryInvalidationQTimer;

public:
    explicit LibraryInvalidationCoordinator(LibraryInvalidationChannel& LibraryInvalidationChannel,
                                            Infrastructure::Persistence::Store::Library::LibraryClockStore& LibraryClockStore,
                                            QObject* Parent = nullptr)
        : QObject{ Parent }
        , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_LibraryInvalidationChannel{ LibraryInvalidationChannel }
        , m_LibraryClockStore{ LibraryClockStore }
        , m_LibraryInvalidationQTimer{} {
        this->m_LibraryInvalidationQTimer.setSingleShot(true);
        QObject::connect(&this->m_LibraryInvalidationQTimer, &QTimer::timeout, this, &LibraryInvalidationCoordinator::HandleScheduledInvalidation);
        this->ScheduleNextLibraryInvalidation();
    }

    void Invalidate(const LibraryInvalidationTargetBitset&) noexcept;
    void InvalidateWithReschedule(const LibraryInvalidationTargetBitset&) noexcept;
    void InvalidateWithRescheduleAndNewSnapshotEpoch(const LibraryInvalidationTargetBitset&) noexcept;

private:
    void HandleScheduledInvalidation() noexcept;
    void ScheduleNextLibraryInvalidation() noexcept;
};

}
#endif
