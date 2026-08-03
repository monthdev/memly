#include "Layer/Infrastructure/Database/DatabaseMigrator.hpp"

#include <duckdb.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "Layer/Infrastructure/Database/ThrowOnDatabaseError.hpp"
#include "Layer/Infrastructure/Sql/Migration/MigrationSql.hpp"
#include "Support/Runtime/Exception/ThrowMemlyException.hpp"

namespace Layer::Infrastructure::Database {

void DatabaseMigrator::ApplyMigrations() {
    ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Sql::Migration::M00_CreateMigrationsLogSql()));
    duckdb::unique_ptr<duckdb::MaterializedQueryResult> ReadMigrationsLogMaterializedQueryResult{ this->m_DatabaseConnection.Query(
        Sql::Migration::ReadMigrationsLogSql()) };
    ThrowOnQueryResultError(*ReadMigrationsLogMaterializedQueryResult);
    std::vector<std::size_t> AppliedMigrationVersionVector{};
    // NOLINTNEXTLINE(custom-memly-no-deduced-variable-type)
    for (const auto& QueryResultRow : *ReadMigrationsLogMaterializedQueryResult) {
        AppliedMigrationVersionVector.emplace_back(static_cast<std::size_t>(QueryResultRow.GetValue<std::uint32_t>(0)));
    }
    const std::size_t AppliedMigrationCount{ AppliedMigrationVersionVector.size() };
    assert(std::ranges::equal(AppliedMigrationVersionVector, std::views::iota(std::size_t{ 1 }, AppliedMigrationCount + std::size_t{ 1 })));
    const std::array<std::string (*)(), 1> MigrationSqlFunctionArray{ &Sql::Migration::M01_CreateSchemaSql };
    const std::size_t AvailableMigrationCount{ MigrationSqlFunctionArray.size() };
    if (AppliedMigrationCount > AvailableMigrationCount) {
        Support::Runtime::Exception::ThrowMemlyException(
            std::initializer_list<std::string_view>{ "Applied migration count exceeds available migration count" });
    }
    if (AppliedMigrationCount < AvailableMigrationCount) {
        duckdb::unique_ptr<duckdb::PreparedStatement> CreateMigrationsLogEntryPreparedStatement{ this->m_DatabaseConnection.Prepare(
            Sql::Migration::CreateMigrationsLogEntrySql()) };
        ThrowOnPreparedStatementError(*CreateMigrationsLogEntryPreparedStatement);
        for (std::size_t MigrationIndex{ AppliedMigrationCount }; MigrationIndex < AvailableMigrationCount; ++MigrationIndex) {
            ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(std::invoke(MigrationSqlFunctionArray.at(MigrationIndex))));
            ThrowOnQueryResultError(*CreateMigrationsLogEntryPreparedStatement->Execute(static_cast<std::uint32_t>(MigrationIndex + 1)));
        }
    }
}

}
