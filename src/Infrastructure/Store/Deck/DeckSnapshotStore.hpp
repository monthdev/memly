#pragma once

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <vector>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/Deck/Query/DeckQuerySql.hpp"
#include "Infrastructure/Store/Deck/DeckSnapshotRecord.hpp"

namespace Infrastructure::Store::Deck {

class DeckSnapshotStore final {
private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadDeckSnapshotRecordsPreparedStatement;

public:
    explicit DeckSnapshotStore(duckdb::Connection& DatabaseConnection)
        : m_ReadDeckSnapshotRecordsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Query::ReadDeckSnapshotRecordsSql()) } {
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_ReadDeckSnapshotRecordsPreparedStatement);
    }

    explicit DeckSnapshotStore(const DeckSnapshotStore&) = delete;
    explicit DeckSnapshotStore(DeckSnapshotStore&&) = delete;
    auto operator=(const DeckSnapshotStore&) -> DeckSnapshotStore& = delete;
    auto operator=(DeckSnapshotStore&&) -> DeckSnapshotStore& = delete;

    [[nodiscard]] auto ReadDeckSnapshotRecords(std::int64_t) -> std::vector<DeckSnapshotRecord>;
};
}
