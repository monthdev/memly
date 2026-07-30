#pragma once

#include <cstdint>
#include <vector>

#include "Infrastructure/Database/DatabaseRuntime.hpp"
#include "Infrastructure/Database/PreparedStatement.hpp"
#include "Infrastructure/Sql/Deck/Query/DeckQuerySql.hpp"
#include "Infrastructure/Store/Deck/DeckSnapshotRecord.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Store::Deck {

class DeckSnapshotStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Infrastructure::Database::DatabaseRuntime& m_DatabaseRuntime;
    Infrastructure::Database::PreparedStatement m_ReadDeckSnapshotRecordsPreparedStatement;

public:
    explicit DeckSnapshotStore(Infrastructure::Database::DatabaseRuntime& DatabaseRuntime)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_DatabaseRuntime{ DatabaseRuntime }
        , m_ReadDeckSnapshotRecordsPreparedStatement{ DatabaseRuntime.PrepareStatement(Infrastructure::Sql::Deck::Query::ReadDeckSnapshotRecordsSql()) } {
    }

    [[nodiscard]] auto ReadDeckSnapshotRecords(std::int64_t) -> std::vector<DeckSnapshotRecord>;
};
}
