// Temporarily disabled during library invalidation control path refactor.
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if 0
#include "Memly/Repository/LibraryRepository.hpp"

#include <duckdb.hpp>

#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>

#include "Memly/Database/DatabaseRuntime.hpp"
#include "_LibrarySql.hpp"

namespace Memly::Repository {

LibraryRepository::LibraryRepository(Database::DatabaseRuntime& DatabaseRuntime)
    : m_DatabaseRuntime{ DatabaseRuntime }
    , m_SelectNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement{
        DatabaseRuntime.PrepareStatement(SelectNextLibraryInvalidationAtMillisecondsSinceEpochSql())
    } {
}

[[nodiscard]] auto LibraryRepository::ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(const std::int64_t AsOfMillisecondsSinceEpoch)
    -> std::optional<std::int64_t> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{
        this->m_DatabaseRuntime.ExecutePreparedStatement(this->m_SelectNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement)
            .WithParameters(AsOfMillisecondsSinceEpoch)
    };
    std::optional<std::int64_t> NextLibraryInvalidationAtMillisecondsSinceEpochOptional{};
    std::size_t ResultRowCount{ 0 };
    while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{ this->m_DatabaseRuntime.FetchNextDataChunk(*QueryResult) }) {
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
