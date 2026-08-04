#include "MigrationSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::DuckDb::Database::Sql::Migration {

[[nodiscard]] auto CreateMigrationsLogSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Bootstrap/CreateMigrationsLog.sql") };
}

[[nodiscard]] auto M01_CreateSchemaSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Version/M01_CreateSchema.sql") };
}

[[nodiscard]] auto SelectAppliedMigrationVersionsSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Select/SelectAppliedMigrationVersions.sql") };
}

[[nodiscard]] auto InsertMigrationLogEntrySql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Insert/InsertMigrationLogEntry.sql") };
}
}
