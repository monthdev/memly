#include "Layer/Infrastructure/Persistence/Store/Deck/DeckSnapshotStore.hpp"

#include <cstdint>
#include <vector>

#include "Layer/Infrastructure/Persistence/Database/QueryResultRowCountRange.hpp"
#include "Layer/Infrastructure/Persistence/Store/Deck/DeckSnapshotRecord.hpp"

namespace Layer::Infrastructure::Persistence::Store::Deck {

[[nodiscard]] auto DeckSnapshotStore::ReadDeckSnapshotRecords(const std::int64_t AsOfMillisecondsSinceEpoch) -> std::vector<DeckSnapshotRecord> {
    return std::vector<DeckSnapshotRecord>{ this->m_SelectDeckSnapshotRecordsPreparedStatement.Execute()
                                                .WithParameters(AsOfMillisecondsSinceEpoch)
                                                .DecodedTo<DeckSnapshotRecord>()
                                                .AssertRowCount(Database::QueryResultRowCountRange::ZeroOrMore()) };
}
}
