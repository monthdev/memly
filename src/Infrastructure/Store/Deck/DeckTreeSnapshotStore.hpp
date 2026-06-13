#pragma once

#include <duckdb.hpp>

#include <QString>
#include <QVector>
#include <QtTypes>
#include <memory>
#include <optional>

#include "Infrastructure/Sql/Deck/Query/DeckQuerySql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::Deck {

class DeckTreeSnapshotStore final {
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

    explicit DeckTreeSnapshotStore(duckdb::Connection& DatabaseConnection)
        : m_ReadDeckTreeSnapshotPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Query::ReadDeckTreeSnapshotSql()) } {
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_ReadDeckTreeSnapshotPreparedStatement);
    }

    ~DeckTreeSnapshotStore() noexcept = default;
    DeckTreeSnapshotStore(const DeckTreeSnapshotStore&) = delete;
    DeckTreeSnapshotStore(DeckTreeSnapshotStore&&) = delete;
    DeckTreeSnapshotStore& operator=(const DeckTreeSnapshotStore&) = delete;
    DeckTreeSnapshotStore& operator=(DeckTreeSnapshotStore&&) = delete;

    [[nodiscard]] QVector<DeckTreeSnapshotRow> ReadDeckTreeSnapshot(const qint64);

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadDeckTreeSnapshotPreparedStatement;
};
}
