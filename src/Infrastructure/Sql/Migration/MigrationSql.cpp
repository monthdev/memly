#include "MigrationSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::Migration {

[[nodiscard]] auto M00_SchemaMigrationsLogSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/M00_SchemaMigrationsLog.sql");
}

[[nodiscard]] auto M01_InitialSchemaSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/M01_InitialSchema.sql");
}

[[nodiscard]] auto CreateSchemaMigrationsLogEntrySql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/CreateSchemaMigrationsLogEntry.sql");
}

[[nodiscard]] auto ReadSchemaMigrationsLogSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/ReadSchemaMigrationsLog.sql");
}
}
