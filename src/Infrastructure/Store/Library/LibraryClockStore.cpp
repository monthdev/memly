#include "Infrastructure/Store/Library/LibraryClockStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>

#include "Infrastructure/Sql/Library/Query/LibraryQuerySql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::Library {

LibraryClockStore::LibraryClockStore(duckdb::Connection& DatabaseConnection)
    : m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement{ DatabaseConnection.Prepare(
          Infrastructure::Sql::Library::Query::ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql()) } {
    Infrastructure::Sql::ThrowOnPreparedStatementError(*m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement);
}

LibraryClockStore::~LibraryClockStore() = default;

[[nodiscard]] std::optional<qint64> LibraryClockStore::ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(const qint64 AsOfMillisecondsSinceEpoch) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement->Execute(
        static_cast<std::int64_t>(AsOfMillisecondsSinceEpoch)) };
    Infrastructure::Sql::ThrowOnQueryResultError(*QueryResult);
    const auto QueryResultIterator{ QueryResult->begin() };
    if ((*QueryResultIterator).IsNull(0)) {
        return std::nullopt;
    }
    return (*QueryResultIterator).GetValue<std::int64_t>(0);
}

}
