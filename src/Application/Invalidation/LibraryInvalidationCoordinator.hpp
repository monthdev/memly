#pragma once

#include <QObject>
#include <QTimer>

#include "Application/Invalidation/LibraryInvalidationChannel.hpp"

namespace Infrastructure::Store::Library {
class LibraryClockStore;
}

namespace Application::Invalidation {

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

    ~LibraryInvalidationCoordinator() override = default;
    LibraryInvalidationCoordinator(const LibraryInvalidationCoordinator&) = delete;
    LibraryInvalidationCoordinator(LibraryInvalidationCoordinator&&) = delete;
    LibraryInvalidationCoordinator& operator=(const LibraryInvalidationCoordinator&) = delete;
    LibraryInvalidationCoordinator& operator=(LibraryInvalidationCoordinator&&) = delete;

    void Invalidate(const LibraryInvalidationTargetBitset&);
    void InvalidateWithReschedule(const LibraryInvalidationTargetBitset&);
    void InvalidateWithRescheduleAndCurrentSnapshotEpoch(const LibraryInvalidationTargetBitset&);

private:
    LibraryInvalidationChannel& m_LibraryInvalidationChannel;
    Infrastructure::Store::Library::LibraryClockStore& m_LibraryClockStore;
    QTimer m_LibraryInvalidationQTimer;

    void HandleScheduledInvalidation();
    void ScheduleNextLibraryInvalidation();
};

}
