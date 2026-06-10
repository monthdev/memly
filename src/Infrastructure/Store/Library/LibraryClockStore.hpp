#pragma once

#include <duckdb.hpp>

#include <QtTypes>
#include <memory>
#include <optional>

#include "Infrastructure/Sql/Library/Query/LibraryQuerySql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

namespace Infrastructure::Store::Library {

class LibraryClockStore final {
public:
    explicit LibraryClockStore(duckdb::Connection& DatabaseConnection)
        : m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::Library::Query::ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql()) } {
        Infrastructure::Sql::ThrowOnPreparedStatementError(*m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement);
    }

    ~LibraryClockStore() = default;

    [[nodiscard]] std::optional<qint64> ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(const qint64);

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement;
};

}
