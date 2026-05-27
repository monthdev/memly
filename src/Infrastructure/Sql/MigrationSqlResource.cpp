#include "MigrationSqlResource.hpp"

#include "SqlResourceBytes.hpp"

namespace Infrastructure::Sql {

[[nodiscard]] std::string M00_SchemaMigrationsLogSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/M00_SchemaMigrationsLog.sql");
}

[[nodiscard]] std::string M01_InitialSchemaSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/M01_InitialSchema.sql");
}

[[nodiscard]] std::string CreateSchemaMigrationsLogEntrySql() {
    return ReadSqlResourceBytes(":/Sql/Migration/CreateSchemaMigrationsLogEntry.sql");
}

[[nodiscard]] std::string ReadSchemaMigrationsLogSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/ReadSchemaMigrationsLog.sql");
}
}
