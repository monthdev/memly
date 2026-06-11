#pragma once

#include <QString>
#include <QVector>
#include <QtTypes>
#include <optional>

namespace Infrastructure::Store::Deck {
class DeckTreeStore;
}

namespace Application::Service::Deck {

class DeckTreeService final {
public:
    struct DeckTreeRow {
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

    explicit DeckTreeService(Infrastructure::Store::Deck::DeckTreeStore& DeckTreeStore) noexcept
        : m_DeckTreeStore{ DeckTreeStore } {
    }

    ~DeckTreeService() noexcept = default;
    DeckTreeService(const DeckTreeService&) = delete;
    DeckTreeService(DeckTreeService&&) = delete;
    DeckTreeService& operator=(const DeckTreeService&) = delete;
    DeckTreeService& operator=(DeckTreeService&&) = delete;

    [[nodiscard]] QVector<DeckTreeRow> ReadDeckTreeSnapshot(qint64);

private:
    Infrastructure::Store::Deck::DeckTreeStore& m_DeckTreeStore;
};

}
