#include "Infrastructure/Database/DatabaseRuntime.hpp"

#include <duckdb.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Database/TransactionRunner.hpp"
#include "Infrastructure/Sql/Migration/MigrationSql.hpp"
#include "Infrastructure/Sql/Seed/SeedSql.hpp"
#include "Support/Runtime/ThrowMemlyException.hpp"

namespace Infrastructure::Database {

[[nodiscard]] auto DatabaseRuntime::GetDatabaseConnection() noexcept -> duckdb::Connection& {
    return m_DatabaseConnection;
}

[[nodiscard]] auto DatabaseRuntime::GetTransactionRunner() noexcept -> TransactionRunner& {
    return m_TransactionRunner;
}

void DatabaseRuntime::BootstrapDatabase() {
    m_TransactionRunner.TransactionWrapper([this]() -> void {
        ApplySchemaMigrations();
        SeedTableDefaults();
    });
}

void DatabaseRuntime::ApplySchemaMigrations() {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(Infrastructure::Sql::Migration::M00_SchemaMigrationsLogSql()) };
    ThrowOnQueryResultError(*QueryResult);
    QueryResult = m_DatabaseConnection.Query(Infrastructure::Sql::Migration::ReadSchemaMigrationsLogSql());
    ThrowOnQueryResultError(*QueryResult);
    std::vector<std::size_t> AppliedMigrationVersionSequenceVector{};
    // NOLINTNEXTLINE(custom-memly-no-deduced-variable-type)
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        // NOLINTNEXTLINE(custom-memly-no-deduced-variable-type)
        const auto& QueryResultRow{ *QueryResultIterator };
        AppliedMigrationVersionSequenceVector.push_back(QueryResultRow.GetValue<std::uint32_t>(0));
    }
    assert(std::ranges::equal(AppliedMigrationVersionSequenceVector,
                              std::views::iota(std::size_t{ 1 }, AppliedMigrationVersionSequenceVector.size() + std::size_t{ 1 })));
    const std::array<std::reference_wrapper<std::string()>, 1> MigrationSqlFunctionArray{ Infrastructure::Sql::Migration::M01_InitialSchemaSql };
    if (AppliedMigrationVersionSequenceVector.size() > MigrationSqlFunctionArray.size()) {
        Support::Runtime::ThrowMemlyException("Unexpected number of applied migrations");
    }
    for (std::size_t UnappliedMigrationVersionIndex{ AppliedMigrationVersionSequenceVector.size() };
         UnappliedMigrationVersionIndex not_eq MigrationSqlFunctionArray.size();
         ++UnappliedMigrationVersionIndex) {
        const std::string& MigrationSql{ std::invoke(MigrationSqlFunctionArray.at(UnappliedMigrationVersionIndex)) };
        QueryResult = m_DatabaseConnection.Query(MigrationSql);
        ThrowOnQueryResultError(*QueryResult);
        QueryResult = m_DatabaseConnection.Query(Infrastructure::Sql::Migration::CreateSchemaMigrationsLogEntrySql(),
                                                 static_cast<std::uint32_t>(UnappliedMigrationVersionIndex + 1));
        ThrowOnQueryResultError(*QueryResult);
    }
}

void DatabaseRuntime::SeedTableDefaults() {
    const std::array<std::reference_wrapper<std::string()>, 3> SeedSqlFunctionArray{ Infrastructure::Sql::Seed::CreateDefaultFsrs7SchedulerSql,
                                                                                     Infrastructure::Sql::Seed::CreateDefaultFsrs7SettingsSql,
                                                                                     Infrastructure::Sql::Seed::CreateDefaultDeckSettingsSql };
    for (const std::reference_wrapper<std::string()>& SeedSqlFunction : SeedSqlFunctionArray) {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(std::invoke(SeedSqlFunction)) };
        ThrowOnQueryResultError(*QueryResult);
    }
}

}
