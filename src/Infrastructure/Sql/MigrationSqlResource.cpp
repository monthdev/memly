#include "MigrationSqlResource.hpp"

#include "SqlResourceBytes.hpp"

namespace Infrastructure::Sql {

[[nodiscard]] std::string M00_MigrationsTableSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/M00_MigrationsTable.sql");
}

[[nodiscard]] std::string M01_InitialSchemaSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/M01_InitialSchema.sql");
}

[[nodiscard]] std::string CreateMigrationSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/CreateMigration.sql");
}

[[nodiscard]] std::string ReadMigrationsTableSql() {
    return ReadSqlResourceBytes(":/Sql/Migration/ReadMigrationsTable.sql");
}
}
