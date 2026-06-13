#pragma once

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <vector>

#include "Domain/Deck/DeckTreeSnapshotNodeData.hpp"
#include "Infrastructure/Sql/Deck/Query/DeckQuerySql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::Deck {

class DeckTreeSnapshotStore final {
public:
    explicit DeckTreeSnapshotStore(duckdb::Connection& DatabaseConnection)
        : m_ReadDeckTreeSnapshotPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Query::ReadDeckTreeSnapshotSql()) } {
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_ReadDeckTreeSnapshotPreparedStatement);
    }

    ~DeckTreeSnapshotStore() noexcept = default;
    DeckTreeSnapshotStore(const DeckTreeSnapshotStore&) = delete;
    DeckTreeSnapshotStore(DeckTreeSnapshotStore&&) = delete;
    DeckTreeSnapshotStore& operator=(const DeckTreeSnapshotStore&) = delete;
    DeckTreeSnapshotStore& operator=(DeckTreeSnapshotStore&&) = delete;

    [[nodiscard]] std::vector<Domain::Deck::DeckTreeSnapshotNodeData> ReadDeckTreeSnapshot(std::int64_t);

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadDeckTreeSnapshotPreparedStatement;
};
}
