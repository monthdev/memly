#pragma once

#include <duckdb.hpp>

#include <cstdint>
#include <memory>
#include <optional>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/Library/Query/LibraryQuerySql.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Store::Library {

class LibraryClockStore final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    std::unique_ptr<duckdb::PreparedStatement> m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement;

public:
    explicit LibraryClockStore(duckdb::Connection& DatabaseConnection)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement{ DatabaseConnection.Prepare(
              Infrastructure::Sql::Library::Query::ReadNextLibraryInvalidationAtMillisecondsSinceEpochSql()) } {
        Infrastructure::Database::ThrowOnPreparedStatementError(*m_ReadNextLibraryInvalidationAtMillisecondsSinceEpochPreparedStatement);
    }

    [[nodiscard]] auto ReadNextLibraryInvalidationAtMillisecondsSinceEpoch(std::int64_t) -> std::optional<std::int64_t>;
};

}
