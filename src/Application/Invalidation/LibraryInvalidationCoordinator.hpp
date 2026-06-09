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
    LibraryInvalidationCoordinator(LibraryInvalidationChannel&, Infrastructure::Store::Library::LibraryClockStore&, QObject* Parent = nullptr);

    void Invalidate(LibraryInvalidationTargetBitset);
    void InvalidateWithReschedule(LibraryInvalidationTargetBitset);
    void InvalidateWithRescheduleAndCurrentSnapshotEpoch(LibraryInvalidationTargetBitset);

private:
    LibraryInvalidationChannel& m_LibraryInvalidationChannel;
    Infrastructure::Store::Library::LibraryClockStore& m_LibraryClockStore;
    QTimer m_LibraryInvalidationQTimer;

    void HandleScheduledInvalidation();
    void ScheduleNextLibraryInvalidation();
};

}
