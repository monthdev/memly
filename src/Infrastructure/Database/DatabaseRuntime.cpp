#include "Infrastructure/Database/DatabaseRuntime.hpp"

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "Infrastructure/Database/SqlExecutionGuard.hpp"
#include "Infrastructure/Sql/Migration/MigrationSql.hpp"
#include "Infrastructure/Sql/Seed/SeedSql.hpp"
#include "Support/Runtime/Crash.hpp"

namespace Infrastructure::Database {

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
    std::vector<std::size_t> AppliedMigrationVersionVector{};
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        AppliedMigrationVersionVector.push_back(QueryResultRow.GetValue<std::uint32_t>(0));
    }
    for (std::size_t I{ 0 }; I not_eq AppliedMigrationVersionVector.size(); ++I) {
        if (AppliedMigrationVersionVector.at(I) not_eq I + 1) {
            Support::Runtime::ThrowError("Unexpected applied migration version order");
        }
    }
    std::array<std::reference_wrapper<std::string()>, 1> MigrationSqlFunctionArray{ Infrastructure::Sql::Migration::M01_InitialSchemaSql };
    if (AppliedMigrationVersionVector.size() > MigrationSqlFunctionArray.size()) {
        Support::Runtime::ThrowError("Unexpected number of applied migrations");
    }
    for (std::size_t UnappliedMigrationVersionIndex{ AppliedMigrationVersionVector.size() };
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
    std::array<std::reference_wrapper<std::string()>, 3> SeedSqlFunctionArray{ Infrastructure::Sql::Seed::CreateDefaultFsrs7SchedulerSql,
                                                                               Infrastructure::Sql::Seed::CreateDefaultFsrs7SettingsSql,
                                                                               Infrastructure::Sql::Seed::CreateDefaultDeckSettingsSql };
    for (const std::reference_wrapper<std::string()>& SeedSqlFunction : SeedSqlFunctionArray) {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DatabaseConnection.Query(std::invoke(SeedSqlFunction)) };
        ThrowOnQueryResultError(*QueryResult);
    }
}

}
