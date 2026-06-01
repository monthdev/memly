#include "Infrastructure/Store/Deck/DeckTreeStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>

#include "Infrastructure/Sql/Query/Deck/DeckQuerySql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::Deck {

DeckTreeStore::DeckTreeStore(duckdb::Connection& DatabaseConnection)
    : m_ReadDeckTreeSnapshotPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Query::Deck::ReadDeckTreeSnapshotSql()) } {
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_ReadDeckTreeSnapshotPreparedStatement);
}

DeckTreeStore::~DeckTreeStore() = default;

[[nodiscard]] QVector<DeckTreeStore::DeckTreeRow> DeckTreeStore::ReadDeckTreeSnapshot(const qint64 AsOfMillisecondsSinceEpoch) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadDeckTreeSnapshotPreparedStatement->Execute(static_cast<std::int64_t>(AsOfMillisecondsSinceEpoch)) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    QVector<DeckTreeRow> DeckTreeRowQVector{};
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        DeckTreeRowQVector.emplace_back(QString{ QueryResultRow.GetValue<std::string>(0).c_str() },
                                        QueryResultRow.IsNull(1) ? std::nullopt :
                                                                   std::make_optional(QString{ QueryResultRow.GetValue<std::string>(1).c_str() }),
                                        QString{ QueryResultRow.GetValue<std::string>(2).c_str() },
                                        QueryResultRow.GetValue<std::int64_t>(3),
                                        QueryResultRow.IsNull(4) ? std::nullopt : std::make_optional<qint64>(QueryResultRow.GetValue<std::int64_t>(4)),
                                        static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(5)),
                                        static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(6)),
                                        static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(7)),
                                        static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(8)),
                                        static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(9)),
                                        static_cast<quint32>(QueryResultRow.GetValue<std::uint32_t>(10)),
                                        static_cast<quint8>(QueryResultRow.GetValue<std::uint8_t>(11)));
    }
    return DeckTreeRowQVector;
}
}
