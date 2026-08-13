#include "MigrationSql.hpp"

#include <string>

#include "Memly/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::Database::Sql {

[[nodiscard]] auto CreateMigrationsLogSql() -> std::string {
    return std::string{ Support::QtApp::ReadQtAppResourceBytes(":/Sql/Bootstrap/CreateMigrationsLog.sql") };
}

[[nodiscard]] auto M01_CreateSchemaSql() -> std::string {
    return std::string{ Support::QtApp::ReadQtAppResourceBytes(":/Sql/Version/M01_CreateSchema.sql") };
}

[[nodiscard]] auto M02_SeedTableDefaultsSql() -> std::string {
    return std::string{ Support::QtApp::ReadQtAppResourceBytes(":/Sql/Version/M02_SeedTableDefaults.sql") };
}

[[nodiscard]] auto SelectAppliedMigrationVersionsSql() -> std::string {
    return std::string{ Support::QtApp::ReadQtAppResourceBytes(":/Sql/Select/SelectAppliedMigrationVersions.sql") };
}

[[nodiscard]] auto InsertMigrationLogEntrySql() -> std::string {
    return std::string{ Support::QtApp::ReadQtAppResourceBytes(":/Sql/Insert/InsertMigrationLogEntry.sql") };
}
}
