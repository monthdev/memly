// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Infrastructure/Store/Library/LibraryClockStore.hpp"

#include <duckdb.hpp>

#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>

#include "Infrastructure/Database/DatabaseRuntime.hpp"

namespace Infrastructure::Store::Library {

[[nodiscard]] auto LibraryClockStore::ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(const std::int64_t AsOfMillisecondsSinceEpoch)
    -> std::optional<std::int64_t> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{
        m_DatabaseRuntime.ExecutePreparedStatement(m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement)
            .WithParameters(AsOfMillisecondsSinceEpoch)
    };
    std::optional<std::int64_t> NextLibraryInvalidationAtMillisecondsSinceEpochOptional{};
    std::size_t ResultRowCount{ 0 };
    while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{ m_DatabaseRuntime.FetchNextDataChunk(*QueryResult) }) {
        for (duckdb::idx_t RowIndex{ 0 }; RowIndex < DataChunk->size(); ++RowIndex) {
            ++ResultRowCount;
            const duckdb::Value NextLibraryInvalidationAtMillisecondsSinceEpochValue{ DataChunk->GetValue(0, RowIndex) };
            if (not NextLibraryInvalidationAtMillisecondsSinceEpochValue.IsNull()) {
                NextLibraryInvalidationAtMillisecondsSinceEpochOptional =
                    NextLibraryInvalidationAtMillisecondsSinceEpochValue.GetValue<std::int64_t>();
            }
        }
    }
    assert(ResultRowCount == 1);
    return NextLibraryInvalidationAtMillisecondsSinceEpochOptional;
}

}
#endif
