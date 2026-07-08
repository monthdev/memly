#pragma once

#include <QObject>
#include <QTimer>

#include "Application/Invalidation/LibraryInvalidationTarget.hpp"

namespace Infrastructure::Store::Library {
class LibraryClockStore;
}

namespace Application::Invalidation {

class LibraryInvalidationChannel;

class LibraryInvalidationCoordinator final : public QObject {
    Q_OBJECT

public:
    LibraryInvalidationCoordinator(LibraryInvalidationChannel& LibraryInvalidationChannel,
                                   Infrastructure::Store::Library::LibraryClockStore& LibraryClockStore,
                                   QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_LibraryInvalidationChannel{ LibraryInvalidationChannel }
        , m_LibraryClockStore{ LibraryClockStore }
        , m_LibraryInvalidationQTimer{} {
        m_LibraryInvalidationQTimer.setSingleShot(true);
        connect(&m_LibraryInvalidationQTimer, &QTimer::timeout, this, &LibraryInvalidationCoordinator::HandleScheduledInvalidation);
        ScheduleNextLibraryInvalidation();
    }

    LibraryInvalidationCoordinator(const LibraryInvalidationCoordinator&) = delete;
    LibraryInvalidationCoordinator(LibraryInvalidationCoordinator&&) = delete;
    LibraryInvalidationCoordinator& operator=(const LibraryInvalidationCoordinator&) = delete;
    LibraryInvalidationCoordinator& operator=(LibraryInvalidationCoordinator&&) = delete;

    void Invalidate(const LibraryInvalidationTargetBitset&) noexcept;
    void InvalidateWithReschedule(const LibraryInvalidationTargetBitset&) noexcept;
    void InvalidateWithRescheduleAndCurrentSnapshotEpoch(const LibraryInvalidationTargetBitset&) noexcept;

private:
    LibraryInvalidationChannel& m_LibraryInvalidationChannel;
    Infrastructure::Store::Library::LibraryClockStore& m_LibraryClockStore;
    QTimer m_LibraryInvalidationQTimer;

    void HandleScheduledInvalidation() noexcept;
    void ScheduleNextLibraryInvalidation() noexcept;
};

}
