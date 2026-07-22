#pragma once

#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "Application/Invalidation/LibraryInvalidationTarget.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Store::Library {
class LibraryClockStore;
}

namespace Application::Invalidation {

class LibraryInvalidationChannel;

class LibraryInvalidationCoordinator final : public QObject, private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    Q_OBJECT
private:
    LibraryInvalidationChannel& m_LibraryInvalidationChannel;
    Infrastructure::Store::Library::LibraryClockStore& m_LibraryClockStore;
    QTimer m_LibraryInvalidationQTimer;

public:
    explicit LibraryInvalidationCoordinator(LibraryInvalidationChannel& LibraryInvalidationChannel,
                                            Infrastructure::Store::Library::LibraryClockStore& LibraryClockStore,
                                            QObject* Parent = nullptr)
        : QObject{ Parent }
        , Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_LibraryInvalidationChannel{ LibraryInvalidationChannel }
        , m_LibraryClockStore{ LibraryClockStore }
        , m_LibraryInvalidationQTimer{} {
        m_LibraryInvalidationQTimer.setSingleShot(true);
        connect(&m_LibraryInvalidationQTimer, &QTimer::timeout, this, &LibraryInvalidationCoordinator::HandleScheduledInvalidation);
        ScheduleNextLibraryInvalidation();
    }

    void Invalidate(const LibraryInvalidationTargetBitset&) noexcept;
    void InvalidateWithReschedule(const LibraryInvalidationTargetBitset&) noexcept;
    void InvalidateWithRescheduleAndCurrentSnapshotEpoch(const LibraryInvalidationTargetBitset&) noexcept;

private:
    void HandleScheduledInvalidation() noexcept;
    void ScheduleNextLibraryInvalidation() noexcept;
};

}
