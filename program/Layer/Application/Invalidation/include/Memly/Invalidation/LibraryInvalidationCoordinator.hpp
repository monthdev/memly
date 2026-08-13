// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#pragma once

#include <qobject.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "Memly/Invalidation/LibraryInvalidationTarget.hpp"
#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Repository {
class LibraryRepository;
}

namespace Layer::Application::Invalidation {

class LibraryInvalidationChannel;

class LibraryInvalidationCoordinator final : public QObject, private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    Q_OBJECT
private:
    LibraryInvalidationChannel& m_LibraryInvalidationChannel;
    Infrastructure::Repository::LibraryRepository& m_LibraryRepository;
    QTimer m_LibraryInvalidationQTimer;

public:
    explicit LibraryInvalidationCoordinator(LibraryInvalidationChannel&,
                                            Infrastructure::Repository::LibraryRepository&,
                                            QObject* = nullptr);

    void Invalidate(const LibraryInvalidationTargetBitset&) noexcept;
    void InvalidateWithReschedule(const LibraryInvalidationTargetBitset&) noexcept;
    void InvalidateWithRescheduleAndNewSnapshotEpoch(const LibraryInvalidationTargetBitset&) noexcept;

private:
    void HandleScheduledInvalidation() noexcept;
    void ScheduleNextLibraryInvalidation() noexcept;
};

}
#endif
