#include "MigrationSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::Persistence::Sql::Migration {

[[nodiscard]] auto M00_CreateMigrationsLogSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/M00_CreateMigrationsLog.sql") };
}

[[nodiscard]] auto M01_CreateSchemaSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/M01_CreateSchema.sql") };
}

[[nodiscard]] auto CreateMigrationsLogEntrySql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/CreateMigrationsLogEntry.sql") };
}

[[nodiscard]] auto ReadMigrationsLogSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/ReadMigrationsLog.sql") };
}
}
