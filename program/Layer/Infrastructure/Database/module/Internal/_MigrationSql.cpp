module;

#include <string>

module Memly.Database.Internal.MigrationSql;

import Memly.QtApp.QtAppResourceBytes;

namespace Memly::Database::Internal {

[[nodiscard]] auto CreateMigrationsLogSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Bootstrap/CreateMigrationsLog.sql") };
}

[[nodiscard]] auto M01CreateSchemaSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Version/M01_CreateSchema.sql") };
}

[[nodiscard]] auto M02SeedTableDefaultsSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Version/M02_SeedTableDefaults.sql") };
}

[[nodiscard]] auto SelectAppliedMigrationVersionsSql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Select/SelectAppliedMigrationVersions.sql") };
}

[[nodiscard]] auto InsertMigrationLogEntrySql() -> std::string {
    return std::string{ QtApp::ReadQtAppResourceBytes(":/Sql/Insert/InsertMigrationLogEntry.sql") };
}
}
