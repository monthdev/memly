#include "_MigrationSql.hpp"

#include <string>

#include "Memly/QtApp/QtAppResourceBytes.hpp"

namespace Memly::Database {

[[nodiscard]] auto i_CreateMigrationsLogSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Bootstrap/CreateMigrationsLog.sql") };
}

[[nodiscard]] auto i_M01_CreateSchemaSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Version/M01_CreateSchema.sql") };
}

[[nodiscard]] auto i_M02_SeedTableDefaultsSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Version/M02_SeedTableDefaults.sql") };
}

[[nodiscard]] auto i_SelectAppliedMigrationVersionsSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Select/SelectAppliedMigrationVersions.sql") };
}

[[nodiscard]] auto i_InsertMigrationLogEntrySql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Insert/InsertMigrationLogEntry.sql") };
}
}
