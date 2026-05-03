#include "Infrastructure/Store/LibraryClockStore.hpp"

#include <duckdb.hpp>

#include <memory>

#include "Infrastructure/Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Infrastructure::Store {

[[nodiscard]] std::optional<qint64> LibraryClockStore::ReadNextLibraryRefreshAtMillisecondsSinceEpoch(const qint64 AsOfMillisecondsSinceEpoch) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Infrastructure::Sql::ReadNextLibraryRefreshAtMillisecondsSinceEpochSql(),
                                                                                 AsOfMillisecondsSinceEpoch) };
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
