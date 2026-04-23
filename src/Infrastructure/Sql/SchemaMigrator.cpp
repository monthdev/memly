#include "SchemaMigrator.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <functional>

#include "Infrastructure/Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Infrastructure::Sql {
namespace {
void ApplySchemaMigrations(duckdb::Connection& DatabaseConnection) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ DatabaseConnection.Query(M00_MigrationsTableSql()) };
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    QueryResult = DatabaseConnection.Query(ReadMigrationsTableSql());
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    std::vector<std::size_t> AppliedMigrationVersionVector{};
    for (auto QueryResultIterator{ QueryResult->begin() }; QueryResultIterator not_eq QueryResult->end(); ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        AppliedMigrationVersionVector.push_back(QueryResultRow.GetValue<std::uint32_t>(0));
    }
    for (std::size_t I{ 0 }; I not_eq AppliedMigrationVersionVector.size(); ++I) {
        if (AppliedMigrationVersionVector.at(I) not_eq I + 1) {
            Support::ThrowError("Unexpected applied migration version order");
        }
    }
    std::array<std::reference_wrapper<std::string()>, 1> MigrationSqlFunctionArray{ M01_InitialSchemaSql };
    if (AppliedMigrationVersionVector.size() > MigrationSqlFunctionArray.size()) {
        Support::ThrowError("Unexpected number of applied migrations");
    }
    for (std::size_t UnappliedMigrationVersionIndex{ AppliedMigrationVersionVector.size() };
         UnappliedMigrationVersionIndex not_eq MigrationSqlFunctionArray.size();
         ++UnappliedMigrationVersionIndex) {
        const std::string& MigrationSql{ MigrationSqlFunctionArray.at(UnappliedMigrationVersionIndex)() };
        QueryResult = DatabaseConnection.Query(MigrationSql);
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
        QueryResult = DatabaseConnection.Query(CreateMigrationSql(), static_cast<std::uint32_t>(UnappliedMigrationVersionIndex + 1));
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
    }
}

void SeedTableDefaults(duckdb::Connection& DatabaseConnection) {
    std::array<std::reference_wrapper<std::string()>, 3> SeedSqlFunctionArray{ CreateDefaultFsrs7SchedulerSql,
                                                                               CreateDefaultFsrs7SettingsSql,
                                                                               CreateDefaultDeckSettingsSql };
    for (const auto& SeedSqlFunction : SeedSqlFunctionArray) {
        std::unique_ptr<duckdb::QueryResult> QueryResult{ DatabaseConnection.Query(SeedSqlFunction()) };
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
    }
}
}

void RunDatabaseBootstrap(duckdb::Connection& DatabaseConnection) {
    ApplySchemaMigrations(DatabaseConnection);
    SeedTableDefaults(DatabaseConnection);
}
}
