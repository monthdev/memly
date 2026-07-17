#include "Infrastructure/Store/Deck/DeckSnapshotStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::Deck {

[[nodiscard]] std::vector<DeckSnapshotStore::DeckSnapshotRecord> DeckSnapshotStore::ReadDeckSnapshotRecords(const std::int64_t AsOfMillisecondsSinceEpoch) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadDeckSnapshotRecordsPreparedStatement->Execute(AsOfMillisecondsSinceEpoch) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    std::vector<DeckSnapshotRecord> DeckSnapshotRecordVector{};
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        DeckSnapshotRecordVector.emplace_back(QueryResultRow.GetValue<std::string>(0),
                                              QueryResultRow.IsNull(1) ? std::nullopt : std::make_optional(QueryResultRow.GetValue<std::string>(1)),
                                              QueryResultRow.GetValue<std::string>(2),
                                              QueryResultRow.GetValue<std::int64_t>(3),
                                              QueryResultRow.IsNull(4) ? std::nullopt : std::make_optional(QueryResultRow.GetValue<std::int64_t>(4)),
                                              QueryResultRow.GetValue<std::uint32_t>(5),
                                              QueryResultRow.GetValue<std::uint32_t>(6),
                                              QueryResultRow.GetValue<std::uint32_t>(7),
                                              QueryResultRow.GetValue<std::uint8_t>(8));
    }
    return DeckSnapshotRecordVector;
}
}
