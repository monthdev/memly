#include "Layer/Infrastructure/Store/Deck/DeckSnapshotStore.hpp"

#include <cstdint>
#include <vector>

#include "Layer/Infrastructure/Database/QueryResultRowCountRange.hpp"
#include "Layer/Infrastructure/Store/Deck/DeckSnapshotRecord.hpp"

namespace Layer::Infrastructure::Store::Deck {

[[nodiscard]] auto DeckSnapshotStore::ReadDeckSnapshotRecords(const std::int64_t AsOfMillisecondsSinceEpoch) -> std::vector<DeckSnapshotRecord> {
    return std::vector<DeckSnapshotRecord>{ this->m_ReadDeckSnapshotRecordsPreparedStatement.Execute()
                                                .WithParameters(AsOfMillisecondsSinceEpoch)
                                                .DecodedTo<DeckSnapshotRecord>()
                                                .AssertRowCount(Database::QueryResultRowCountRange::ZeroOrMore()) };
}
}
