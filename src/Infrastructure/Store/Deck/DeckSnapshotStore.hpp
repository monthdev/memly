#pragma once

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <vector>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/Deck/Query/DeckQuerySql.hpp"
#include "Infrastructure/Store/Deck/DeckSnapshotRecord.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Store::Deck {

class DeckSnapshotStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadDeckSnapshotRecordsPreparedStatement;

public:
    explicit DeckSnapshotStore(duckdb::Connection& DatabaseConnection)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_ReadDeckSnapshotRecordsPreparedStatement{ DatabaseConnection.Prepare(Infrastructure::Sql::Deck::Query::ReadDeckSnapshotRecordsSql()) } {
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_ReadDeckSnapshotRecordsPreparedStatement);
    }

    [[nodiscard]] auto ReadDeckSnapshotRecords(std::int64_t) -> std::vector<DeckSnapshotRecord>;
};
}
