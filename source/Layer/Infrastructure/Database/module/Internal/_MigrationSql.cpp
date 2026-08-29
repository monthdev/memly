module;

#include <string>

module Memly.Database.Internal.MigrationSql;

import Memly.QtApp.QtAppResourceBytes;

namespace Memly::Database::Internal {

[[nodiscard]] std::string
CreateMigrationsLogSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(
            ":/Sql/Bootstrap/CreateMigrationsLog.sql"
        ),
    };
}

[[nodiscard]] std::string
M01CreateSchemaSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(":/Sql/Version/M01_CreateSchema.sql"),
    };
}

[[nodiscard]] std::string
M02SeedTableDefaultsSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(
            ":/Sql/Version/M02_SeedTableDefaults.sql"
        ),
    };
}

[[nodiscard]] std::string
SelectAppliedMigrationVersionsSql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(
            ":/Sql/Select/SelectAppliedMigrationVersions.sql"
        ),
    };
}

[[nodiscard]] std::string
InsertMigrationLogEntrySql() {
    return std::string{
        QtApp::ReadQtAppResourceBytes(
            ":/Sql/Insert/InsertMigrationLogEntry.sql"
        ),
    };
}
}
