#pragma once

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/Library/Query/LibraryQuerySql.hpp"

namespace Infrastructure::Store::Library {

class LibraryClockStore final {
private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement;

public:
    explicit LibraryClockStore(duckdb::Connection& DatabaseConnection)
        : m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::Library::Query::ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql()) } {
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement);
    }

    explicit LibraryClockStore(const LibraryClockStore&) = delete;
    explicit LibraryClockStore(LibraryClockStore&&) = delete;
    auto operator=(const LibraryClockStore&) -> LibraryClockStore& = delete;
    auto operator=(LibraryClockStore&&) -> LibraryClockStore& = delete;

    [[nodiscard]] auto ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(std::int64_t) -> std::optional<std::int64_t>;
};

}
