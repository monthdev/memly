#pragma once

#include <QString>
#include <QVector>
#include <QtTypes>
#include <optional>

namespace Infrastructure::Store::Deck {
class DeckTreeSnapshotStore;
}

namespace Application::Service::Deck {

class DeckTreeSnapshotService final {
public:
    struct DeckTreeSnapshotRow {
        QString m_DeckId;
        std::optional<QString> m_ParentDeckId;
        QString m_DeckName;
        qint64 m_CreatedAtMillisecondsSinceEpoch;
        std::optional<qint64> m_UpdatedAtMillisecondsSinceEpoch;
        quint32 m_SelfDueNowCount;
        quint32 m_SelfByTodayCount;
        quint32 m_SelfTotalCount;
        quint32 m_SubtreeDueNowCount;
        quint32 m_SubtreeByTodayCount;
        quint32 m_SubtreeTotalCount;
        quint8 m_TargetLanguageCode;
    };

    explicit DeckTreeSnapshotService(Infrastructure::Store::Deck::DeckTreeSnapshotStore& DeckTreeSnapshotStore) noexcept
        : m_DeckTreeSnapshotStore{ DeckTreeSnapshotStore } {
    }

    ~DeckTreeSnapshotService() noexcept = default;
    DeckTreeSnapshotService(const DeckTreeSnapshotService&) = delete;
    DeckTreeSnapshotService(DeckTreeSnapshotService&&) = delete;
    DeckTreeSnapshotService& operator=(const DeckTreeSnapshotService&) = delete;
    DeckTreeSnapshotService& operator=(DeckTreeSnapshotService&&) = delete;

    [[nodiscard]] QVector<DeckTreeSnapshotRow> ReadDeckTreeSnapshot(qint64);

private:
    Infrastructure::Store::Deck::DeckTreeSnapshotStore& m_DeckTreeSnapshotStore;
};

}
