#include "Coordinator/LibraryRefreshCoordinator.hpp"

#include <QDateTime>
#include <QtGlobal>
#include <algorithm>
#include <optional>

#include "Infrastructure/Store/LibraryClockStore.hpp"
#include "Support/Fatal.hpp"

namespace Coordinator {

void LibraryRefreshCoordinator::NotifyLibraryMutated(const bool RescheduleNextLibraryRefresh) {
    RunLibraryRefreshCycle(RescheduleNextLibraryRefresh);
}

void LibraryRefreshCoordinator::RunLibraryRefreshCycle(const bool RescheduleNextLibraryRefresh) {
    m_AsOfMillisecondsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
    emit RefreshRequested(m_AsOfMillisecondsSinceEpoch);
    if (RescheduleNextLibraryRefresh) {
        ScheduleNextLibraryRefresh();
    }
}

void LibraryRefreshCoordinator::ScheduleNextLibraryRefresh() {
    Support::TryCatchWrapper([&] {
        m_LibraryRefreshQTimer.stop();
        const std::optional<qint64> NextLibraryRefreshAtMillisecondsSinceEpoch{ m_LibraryClockStore.ReadNextLibraryRefreshAtMillisecondsSinceEpoch(
            m_AsOfMillisecondsSinceEpoch) };
        if (not NextLibraryRefreshAtMillisecondsSinceEpoch.has_value()) {
            return;
        }
        const qint64 LibraryRefreshDelayMilliseconds{ std::max<qint64>(
            0, NextLibraryRefreshAtMillisecondsSinceEpoch.value() - QDateTime::currentMSecsSinceEpoch()) };
        m_LibraryRefreshQTimer.start(static_cast<int>(LibraryRefreshDelayMilliseconds));
    });
}

}
