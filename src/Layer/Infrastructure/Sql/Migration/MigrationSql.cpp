#include "MigrationSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::Sql::Migration {

[[nodiscard]] auto M00_MigrationsLogSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/M00_MigrationsLog.sql");
}

[[nodiscard]] auto M01_InitialSchemaSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/M01_InitialSchema.sql");
}

[[nodiscard]] auto CreateMigrationsLogEntrySql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/CreateMigrationsLogEntry.sql");
}

[[nodiscard]] auto ReadMigrationsLogSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/ReadMigrationsLog.sql");
}
}
