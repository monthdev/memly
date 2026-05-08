#include "Infrastructure/Store/LibraryClockStore.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <memory>

#include "Infrastructure/Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Infrastructure::Store {

LibraryClockStore::LibraryClockStore(duckdb::Connection& DatabaseConnection)
    : m_ReadNextLibraryRefreshAtMillisecondsSinceEpochPreparedStatement{ DatabaseConnection.Prepare(
          Infrastructure::Sql::ReadNextLibraryRefreshAtMillisecondsSinceEpochSql()) } {
    if (m_ReadNextLibraryRefreshAtMillisecondsSinceEpochPreparedStatement->HasError()) {
        Support::ThrowError(m_ReadNextLibraryRefreshAtMillisecondsSinceEpochPreparedStatement->GetError());
    }
}

LibraryClockStore::~LibraryClockStore() = default;

[[nodiscard]] std::optional<qint64> LibraryClockStore::ReadNextLibraryRefreshAtMillisecondsSinceEpoch(const qint64 AsOfMillisecondsSinceEpoch) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_ReadNextLibraryRefreshAtMillisecondsSinceEpochPreparedStatement->Execute(
        static_cast<std::int64_t>(AsOfMillisecondsSinceEpoch)) };
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    const auto QueryResultIterator{ QueryResult->begin() };
    const auto& QueryResultRow{ *QueryResultIterator };
    if (QueryResultRow.IsNull(0)) {
        return std::nullopt;
    }
    return QueryResultRow.GetValue<std::int64_t>(0);
}

}
