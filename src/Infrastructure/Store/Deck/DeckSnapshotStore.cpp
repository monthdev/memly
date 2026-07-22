#include "Infrastructure/Store/Deck/DeckSnapshotStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Store/Deck/DeckSnapshotRecord.hpp"

namespace Infrastructure::Store::Deck {

[[nodiscard]] auto DeckSnapshotStore::ReadDeckSnapshotRecords(const std::int64_t AsOfMillisecondsSinceEpoch) -> std::vector<DeckSnapshotRecord> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadDeckSnapshotRecordsPreparedStatement->Execute(AsOfMillisecondsSinceEpoch) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    std::vector<DeckSnapshotRecord> DeckSnapshotRecordVector{};
    // NOLINTNEXTLINE(custom-memly-no-deduced-variable-type)
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        // NOLINTNEXTLINE(custom-memly-no-deduced-variable-type)
        const auto& QueryResultRow{ *QueryResultIterator };
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        DeckSnapshotRecordVector.emplace_back(QueryResultRow.GetValue<std::string>(0),
                                              QueryResultRow.IsNull(1) ? std::nullopt : std::make_optional(QueryResultRow.GetValue<std::string>(1)),
                                              QueryResultRow.GetValue<std::string>(2),
                                              QueryResultRow.GetValue<std::int64_t>(3),
                                              QueryResultRow.IsNull(4) ? std::nullopt : std::make_optional(QueryResultRow.GetValue<std::int64_t>(4)),
                                              QueryResultRow.GetValue<std::uint32_t>(5),
                                              QueryResultRow.GetValue<std::uint32_t>(6),
                                              QueryResultRow.GetValue<std::uint32_t>(7),
                                              QueryResultRow.GetValue<std::uint8_t>(8));
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }
    return DeckSnapshotRecordVector;
}
}
