#pragma once

#include <cstdint>
#include <vector>

#include "Layer/Infrastructure/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/Sql/Deck/Query/DeckQuerySql.hpp"
#include "Layer/Infrastructure/Store/Deck/DeckSnapshotRecord.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Store::Deck {

class DeckSnapshotStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Database::PreparedStatement m_ReadDeckSnapshotRecordsPreparedStatement;

public:
    explicit DeckSnapshotStore(Database::DatabaseRuntime& DatabaseRuntime)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_ReadDeckSnapshotRecordsPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::Query::ReadDeckSnapshotRecordsSql()) } {
    }

    [[nodiscard]] auto ReadDeckSnapshotRecords(std::int64_t) -> std::vector<DeckSnapshotRecord>;
};
}
