#include "Layer/Infrastructure/Store/Deck/DeckSnapshotStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Layer/Infrastructure/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/Store/Deck/DeckSnapshotRecord.hpp"

namespace Layer::Infrastructure::Store::Deck {

[[nodiscard]] auto DeckSnapshotStore::ReadDeckSnapshotRecords(const std::int64_t AsOfMillisecondsSinceEpoch) -> std::vector<DeckSnapshotRecord> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{
        m_DatabaseRuntime.ExecutePreparedStatement(m_ReadDeckSnapshotRecordsPreparedStatement).WithParameters(AsOfMillisecondsSinceEpoch)
    };
    std::vector<DeckSnapshotRecord> DeckSnapshotRecordVector{};
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{ m_DatabaseRuntime.FetchNextDataChunk(*QueryResult) }) {
        for (duckdb::idx_t RowIndex{ 0 }; RowIndex < DataChunk->size(); ++RowIndex) {
            DeckSnapshotRecordVector.emplace_back(
                DataChunk->GetValue(0, RowIndex).GetValue<std::string>(),
                DataChunk->GetValue(1, RowIndex).IsNull() ? std::nullopt : std::make_optional(DataChunk->GetValue(1, RowIndex).GetValue<std::string>()),
                DataChunk->GetValue(2, RowIndex).GetValue<std::string>(),
                DataChunk->GetValue(3, RowIndex).GetValue<std::int64_t>(),
                DataChunk->GetValue(4, RowIndex).IsNull() ? std::nullopt : std::make_optional(DataChunk->GetValue(4, RowIndex).GetValue<std::int64_t>()),
                DataChunk->GetValue(5, RowIndex).GetValue<std::uint32_t>(),
                DataChunk->GetValue(6, RowIndex).GetValue<std::uint32_t>(),
                DataChunk->GetValue(7, RowIndex).GetValue<std::uint32_t>(),
                DataChunk->GetValue(8, RowIndex).GetValue<std::uint8_t>());
        }
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return DeckSnapshotRecordVector;
}
}
