#include "Layer/Infrastructure/DuckDb/Database/DatabaseMigrator.hpp"

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
#include <utility>
#include <vector>

#include "Layer/Infrastructure/DuckDb/Database/DatabaseRuntime.hpp"
#include "Layer/Infrastructure/DuckDb/Database/Sql/MigrationSql.hpp"
#include "Layer/Infrastructure/DuckDb/Database/ThrowOnDatabaseError.hpp"
#include "Layer/Infrastructure/DuckDb/Database/TransactionRunner.hpp"
#include "Support/Runtime/Exception/ThrowMemlyException.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

DatabaseMigrator::DatabaseMigrator(const std::string& DatabaseFilePath)
    : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_Database{ DatabaseFilePath }
    , m_DatabaseConnection{ this->m_Database } {
}

[[nodiscard]] auto DatabaseMigrator::ApplyMigrations() && -> DatabaseRuntime {
    TransactionRunner{ this->m_DatabaseConnection }.TransactionWrapper([this]() -> void {
        ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Sql::CreateMigrationsLogSql()));
        duckdb::unique_ptr<duckdb::MaterializedQueryResult> AppliedMigrationVersionsMaterializedQueryResult{ this->m_DatabaseConnection.Query(
            Sql::SelectAppliedMigrationVersionsSql()) };
        ThrowOnQueryResultError(*AppliedMigrationVersionsMaterializedQueryResult);
        std::vector<std::size_t> AppliedMigrationVersionVector{};
        // NOLINTNEXTLINE(custom-memly-no-deduced-variable-type)
        for (const auto& QueryResultRow : *AppliedMigrationVersionsMaterializedQueryResult) {
            AppliedMigrationVersionVector.emplace_back(QueryResultRow.GetValue<std::uint32_t>(0));
        }
        const std::size_t AppliedMigrationCount{ AppliedMigrationVersionVector.size() };
        assert(std::ranges::equal(AppliedMigrationVersionVector, std::views::iota(std::size_t{ 1 }, AppliedMigrationCount + std::size_t{ 1 })));
        constexpr std::size_t MigrationSqlFunctionCount{ 2 };
        const std::array<std::string (*)(), MigrationSqlFunctionCount> MigrationSqlFunctionArray{ &Sql::M01_CreateSchemaSql, &Sql::M02_SeedTableDefaultsSql };
        const std::size_t AvailableMigrationCount{ MigrationSqlFunctionArray.size() };
        if (AppliedMigrationCount > AvailableMigrationCount) {
            Support::Runtime::Exception::ThrowMemlyException(
                std::initializer_list<std::string_view>{ "Applied migration count exceeds available migration count" });
        }
        if (AppliedMigrationCount < AvailableMigrationCount) {
            duckdb::unique_ptr<duckdb::PreparedStatement> InsertMigrationLogEntryPreparedStatement{ this->m_DatabaseConnection.Prepare(
                Sql::InsertMigrationLogEntrySql()) };
            ThrowOnPreparedStatementError(*InsertMigrationLogEntryPreparedStatement);
            for (std::size_t MigrationIndex{ AppliedMigrationCount }; MigrationIndex < AvailableMigrationCount; ++MigrationIndex) {
                ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(std::invoke(MigrationSqlFunctionArray.at(MigrationIndex))));
                ThrowOnQueryResultError(*InsertMigrationLogEntryPreparedStatement->Execute(static_cast<std::uint32_t>(MigrationIndex + std::size_t{ 1 })));
            }
        }
    });
    return DatabaseRuntime{ *this->m_Database.instance, std::move(this->m_DatabaseConnection) };
}

}
