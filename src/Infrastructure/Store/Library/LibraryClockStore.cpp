#include "Infrastructure/Store/Library/LibraryClockStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::Library {

[[nodiscard]] auto LibraryClockStore::ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(const std::int64_t AsOfMillisecondsSinceEpoch)
    -> std::optional<std::int64_t> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement->Execute(
        AsOfMillisecondsSinceEpoch) };
    Infrastructure::Database::ThrowOnQueryResultError(*QueryResult);
    const auto QueryResultIterator{ QueryResult->begin() };
    if ((*QueryResultIterator).IsNull(0)) {
        return std::nullopt;
    }
    return (*QueryResultIterator).GetValue<std::int64_t>(0);
}

}
