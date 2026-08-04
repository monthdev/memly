#pragma once

#include <cstdint>
#include <vector>

#include "Layer/Infrastructure/Persistence/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/Persistence/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/Persistence/Sql/Deck/DeckSql.hpp"
#include "Layer/Infrastructure/Persistence/Store/Deck/DeckSnapshotRecord.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Persistence::Store::Deck {

class DeckSnapshotStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Database::PreparedStatement m_SelectDeckSnapshotRecordsPreparedStatement;

public:
    explicit DeckSnapshotStore(Database::DatabaseRuntime& DatabaseRuntime)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_SelectDeckSnapshotRecordsPreparedStatement{ DatabaseRuntime.PrepareStatement(Sql::Deck::SelectDeckSnapshotRecordsSql()) } {
    }

    [[nodiscard]] auto ReadDeckSnapshotRecords(std::int64_t) -> std::vector<DeckSnapshotRecord>;
};
}
