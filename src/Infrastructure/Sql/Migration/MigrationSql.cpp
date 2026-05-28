#include "MigrationSql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Migration {

[[nodiscard]] std::string M00_SchemaMigrationsLogSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/Statement/M00_SchemaMigrationsLog.sql");
}

[[nodiscard]] std::string M01_InitialSchemaSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/Statement/M01_InitialSchema.sql");
}

[[nodiscard]] std::string CreateSchemaMigrationsLogEntrySql() {
    return ReadSqlResourceBytes(":/Sql/Migration/Statement/CreateSchemaMigrationsLogEntry.sql");
}

[[nodiscard]] std::string ReadSchemaMigrationsLogSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/Statement/ReadSchemaMigrationsLog.sql");
}
}
