#include "MigrationSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::Migration {

[[nodiscard]] std::string M00_SchemaMigrationsLogSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/M00_SchemaMigrationsLog.sql");
}

[[nodiscard]] std::string M01_InitialSchemaSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/M01_InitialSchema.sql");
}

[[nodiscard]] std::string CreateSchemaMigrationsLogEntrySql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/CreateSchemaMigrationsLogEntry.sql");
}

[[nodiscard]] std::string ReadSchemaMigrationsLogSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Migration/Statement/ReadSchemaMigrationsLog.sql");
}
}
