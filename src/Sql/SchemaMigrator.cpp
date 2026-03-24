#include "SchemaMigrator.hpp"

#include <duckdb.hpp>

#include <cstdint>
#include <functional>

#include "Sql/SqlResource.hpp"
#include "Support/Fatal.hpp"

namespace Sql {

// namespace {
// void ApplySchemaMigrations(duckdb::Connection& DatabaseConnection);
// void SeedTableDefaults(duckdb::Connection& DatabaseConnection);
// }

void RunDatabaseBootstrap(duckdb::Connection& DatabaseConnection) {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ DatabaseConnection.Query(
        Sql::M00_MigrationsTableSql()) };
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    QueryResult = DatabaseConnection.Query(Sql::ReadMigrationsTableSql());
    if (QueryResult->HasError()) {
        Support::ThrowError(QueryResult->GetError());
    }
    std::vector<std::size_t> AppliedMigrationVersionVector{};
    for (auto QueryResultIterator{ QueryResult->begin() };
         QueryResultIterator != QueryResult->end();
         ++QueryResultIterator) {
        const auto& QueryResultRow{ *QueryResultIterator };
        AppliedMigrationVersionVector.push_back(QueryResultRow.GetValue<std::uint32_t>(0));
    }
    for (std::size_t I{ 0 }; I != AppliedMigrationVersionVector.size(); ++I) {
        if (AppliedMigrationVersionVector.at(I) != I + 1) {
            Support::ThrowError("Unexpected applied migration version order");
        }
    }
    std::array<std::reference_wrapper<std::string()>, 1> MigrationSqlFunctionArray{
        Sql::M01_InitialSchemaSql
    };
    if (AppliedMigrationVersionVector.size() > MigrationSqlFunctionArray.size()) {
        Support::ThrowError("Unexpected number of applied migrations");
    }
    for (std::size_t UnappliedMigrationVersionIndex{ AppliedMigrationVersionVector.size() };
         UnappliedMigrationVersionIndex != MigrationSqlFunctionArray.size();
         ++UnappliedMigrationVersionIndex) {
        const std::string& MigrationSql{ MigrationSqlFunctionArray.at(
            UnappliedMigrationVersionIndex)() };
        QueryResult = DatabaseConnection.Query(MigrationSql);
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
        QueryResult = DatabaseConnection.Query(
            Sql::CreateMigrationSql(),
            static_cast<std::uint32_t>(UnappliedMigrationVersionIndex + 1));
        if (QueryResult->HasError()) {
            Support::ThrowError(QueryResult->GetError());
        }
    }
}
}
