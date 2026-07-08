#pragma once

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/Library/Query/LibraryQuerySql.hpp"

namespace Infrastructure::Store::Library {

class LibraryClockStore final {
public:
    explicit LibraryClockStore(duckdb::Connection& DatabaseConnection)
        : m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::Library::Query::ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql()) } {
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement);
    }

    LibraryClockStore(const LibraryClockStore&) = delete;
    LibraryClockStore(LibraryClockStore&&) = delete;
    LibraryClockStore& operator=(const LibraryClockStore&) = delete;
    LibraryClockStore& operator=(LibraryClockStore&&) = delete;

    [[nodiscard]] std::optional<std::int64_t> ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(std::int64_t);

private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement;
};

}
