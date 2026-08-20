export module Memly.Invalidation:LibraryInvalidationCoordinator;

// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>

import :LibraryInvalidationChannel;
import :LibraryInvalidationTarget;
import Memly.Repository;

export namespace Memly::Invalidation {

class LibraryInvalidationCoordinator final : public QObject {
    Q_OBJECT
private:
    LibraryInvalidationChannel& m_LibraryInvalidationChannel;
    Repository::LibraryRepository& m_LibraryRepository;
    QTimer m_LibraryInvalidationQTimer;

public:
    explicit LibraryInvalidationCoordinator(LibraryInvalidationChannel&,
                                            Repository::LibraryRepository&,
                                            QObject* = nullptr);

    explicit LibraryInvalidationCoordinator(const LibraryInvalidationCoordinator&) = delete;
    auto operator=(const LibraryInvalidationCoordinator&) -> LibraryInvalidationCoordinator& = delete;

    explicit LibraryInvalidationCoordinator(LibraryInvalidationCoordinator&&) = delete;
    auto operator=(LibraryInvalidationCoordinator&&) -> LibraryInvalidationCoordinator& = delete;

    ~LibraryInvalidationCoordinator() noexcept override = default;

    void Invalidate(const LibraryInvalidationTargetBitset&) noexcept;
    void InvalidateWithReschedule(const LibraryInvalidationTargetBitset&) noexcept;
    void InvalidateWithRescheduleAndNewSnapshotEpoch(const LibraryInvalidationTargetBitset&) noexcept;

private:
    void HandleScheduledInvalidation() noexcept;
    void ScheduleNextLibraryInvalidation() noexcept;
};

}
#endif
