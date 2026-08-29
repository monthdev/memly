export module Memly.Invalidation.LibraryInvalidationCoordinator;

// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>

import Memly.Invalidation.LibraryInvalidationChannel;
import Memly.Invalidation.LibraryInvalidationTarget;
import Memly.Repository.LibraryRepository;

export namespace Memly::Invalidation {

class LibraryInvalidationCoordinator final : public QObject {
    Q_OBJECT
private:
    LibraryInvalidationChannel& m_LibraryInvalidationChannel;
    Repository::LibraryRepository& m_LibraryRepository;
    QTimer m_LibraryInvalidationQTimer;

public:
    explicit LibraryInvalidationCoordinator(
        LibraryInvalidationChannel&,
        Repository::LibraryRepository&,
        QObject* = nullptr
    );

    explicit LibraryInvalidationCoordinator(
        const LibraryInvalidationCoordinator&
    ) = delete;
    LibraryInvalidationCoordinator&
    operator=(const LibraryInvalidationCoordinator&) = delete;

    explicit LibraryInvalidationCoordinator(
        LibraryInvalidationCoordinator&&
    ) = delete;
    LibraryInvalidationCoordinator&
    operator=(LibraryInvalidationCoordinator&&) = delete;

    ~LibraryInvalidationCoordinator() noexcept override = default;

    void
    Invalidate(const LibraryInvalidationTargetBitset&) noexcept;

    void
    InvalidateWithReschedule(const LibraryInvalidationTargetBitset&) noexcept;

    void
    InvalidateWithRescheduleAndNewSnapshotEpoch(
        const LibraryInvalidationTargetBitset&
    ) noexcept;

private:
    void
    HandleScheduledInvalidation() noexcept;

    void
    ScheduleNextLibraryInvalidation() noexcept;
};

}
#endif
