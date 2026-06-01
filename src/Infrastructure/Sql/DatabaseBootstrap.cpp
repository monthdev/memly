#include "DatabaseBootstrap.hpp"

#include <duckdb.hpp>

#include <array>
#include <cstdint>
#include <functional>

#include "Infrastructure/Sql/Migration/MigrationSql.hpp"
#include "Infrastructure/Sql/Seed/SeedSql.hpp"
#include "Infrastructure/Sql/SqlExecutionGuard.hpp"
#include "Runtime/Crash.hpp"

namespace Infrastructure::Sql {
namespace {
void ApplySchemaMigrations(duckdb::Connection& DatabaseConnection) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ DatabaseConnection.Query(Migration::M00_SchemaMigrationsLogSql()) };
    ThrowOnQueryResultError(*QueryResult);
    QueryResult = DatabaseConnection.Query(Migration::ReadSchemaMigrationsLogSql());
    ThrowOnQueryResultError(*QueryResult);
    std::vector<std::size_t> AppliedMigrationVersionVector{};
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        AppliedMigrationVersionVector.push_back(QueryResultRow.GetValue<std::uint32_t>(0));
    }
    for (std::size_t I{ 0 }; I not_eq AppliedMigrationVersionVector.size(); ++I) {
        if (AppliedMigrationVersionVector.at(I) not_eq I + 1) {
            Runtime::ThrowError("Unexpected applied migration version order");
        }
    }
    std::array<std::reference_wrapper<std::string()>, 1> MigrationSqlFunctionArray{ Migration::M01_InitialSchemaSql };
    if (AppliedMigrationVersionVector.size() > MigrationSqlFunctionArray.size()) {
        Runtime::ThrowError("Unexpected number of applied migrations");
    }
    for (std::size_t UnappliedMigrationVersionIndex{ AppliedMigrationVersionVector.size() };
         UnappliedMigrationVersionIndex not_eq MigrationSqlFunctionArray.size();
         ++UnappliedMigrationVersionIndex) {
        const std::string& MigrationSql{ MigrationSqlFunctionArray.at(UnappliedMigrationVersionIndex)() };
        QueryResult = DatabaseConnection.Query(MigrationSql);
        ThrowOnQueryResultError(*QueryResult);
        QueryResult = DatabaseConnection.Query(Migration::CreateSchemaMigrationsLogEntrySql(), static_cast<std::uint32_t>(UnappliedMigrationVersionIndex + 1));
        ThrowOnQueryResultError(*QueryResult);
    }
}

void SeedTableDefaults(duckdb::Connection& DatabaseConnection) {
    std::array<std::reference_wrapper<std::string()>, 3> SeedSqlFunctionArray{ Seed::CreateDefaultFsrs7SchedulerSql,
                                                                               Seed::CreateDefaultFsrs7SettingsSql,
                                                                               Seed::CreateDefaultDeckSettingsSql };
    for (const auto& SeedSqlFunction : SeedSqlFunctionArray) {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ DatabaseConnection.Query(SeedSqlFunction()) };
        ThrowOnQueryResultError(*QueryResult);
    }
}
}

void RunDatabaseBootstrap(duckdb::Connection& DatabaseConnection) {
    ApplySchemaMigrations(DatabaseConnection);
    SeedTableDefaults(DatabaseConnection);
}
}
