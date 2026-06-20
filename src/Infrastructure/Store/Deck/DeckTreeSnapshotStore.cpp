#include "Infrastructure/Store/Deck/DeckTreeSnapshotStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::Deck {

[[nodiscard]] std::vector<Domain::Deck::DeckTreeSnapshotNode> DeckTreeSnapshotStore::ReadDeckTreeSnapshotNodes(const std::int64_t AsOfMillisecondsSinceEpoch) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadDeckTreeSnapshotNodesPreparedStatement->Execute(AsOfMillisecondsSinceEpoch) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    std::vector<Domain::Deck::DeckTreeSnapshotNode> DeckTreeSnapshotNodeVector{};
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        DeckTreeSnapshotNodeVector.emplace_back(QueryResultRow.GetValue<std::string>(0),
                                                QueryResultRow.IsNull(1) ? std::nullopt : std::make_optional(QueryResultRow.GetValue<std::string>(1)),
                                                QueryResultRow.GetValue<std::string>(2),
                                                QueryResultRow.GetValue<std::int64_t>(3),
                                                QueryResultRow.IsNull(4) ? std::nullopt : std::make_optional(QueryResultRow.GetValue<std::int64_t>(4)),
                                                QueryResultRow.GetValue<std::uint32_t>(5),
                                                QueryResultRow.GetValue<std::uint32_t>(6),
                                                QueryResultRow.GetValue<std::uint32_t>(7),
                                                QueryResultRow.GetValue<std::uint32_t>(8),
                                                QueryResultRow.GetValue<std::uint32_t>(9),
                                                QueryResultRow.GetValue<std::uint32_t>(10),
                                                QueryResultRow.GetValue<std::uint8_t>(11));
    }
    return DeckTreeSnapshotNodeVector;
}
}
