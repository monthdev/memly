#pragma once

#include <QString>
#include <QVector>
#include <QtTypes>
#include <memory>
#include <optional>

namespace duckdb {
class Connection;
class PreparedStatement;
}

namespace Infrastructure::Store::Deck {

class DeckTreeStore final {
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

    explicit DeckTreeStore(duckdb::Connection&);
    ~DeckTreeStore();

    [[nodiscard]] QVector<DeckTreeRow> ReadDeckTreeSnapshot(const qint64);

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadDeckTreeSnapshotPreparedStatement;
};
}
