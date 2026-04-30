#pragma once

#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QtTypes>

namespace Infrastructure::Store {
class LibraryClockStore;
}

namespace Coordinator {

class LibraryRefreshCoordinator final : public QObject {
    Q_OBJECT

public:
    explicit LibraryRefreshCoordinator(Infrastructure::Store::LibraryClockStore& LibraryClockStore, QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_LibraryClockStore{ LibraryClockStore }
        , m_LibraryRefreshQTimer{}
        , m_AsOfMillisecondsSinceEpoch{ QDateTime::currentMSecsSinceEpoch() } {
        m_LibraryRefreshQTimer.setSingleShot(true);
        connect(&m_LibraryRefreshQTimer, &QTimer::timeout, this, [this] { RunLibraryRefreshCycle(true); });
        ScheduleNextLibraryRefresh();
    }

    [[nodiscard]] qint64 GetAsOfMillisecondsSinceEpoch() const noexcept {
        return m_AsOfMillisecondsSinceEpoch;
    }

    void NotifyLibraryMutated(const bool);

signals:
    void RefreshRequested(const qint64);

private:
    Infrastructure::Store::LibraryClockStore& m_LibraryClockStore;
    QTimer m_LibraryRefreshQTimer;
    qint64 m_AsOfMillisecondsSinceEpoch;

    void RunLibraryRefreshCycle(const bool);
    void ScheduleNextLibraryRefresh();
};

}
