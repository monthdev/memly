#pragma once

#include <string>

namespace Infrastructure::Sql {

[[nodiscard]] std::string M00_SchemaMigrationsLogSql();

[[nodiscard]] std::string M01_InitialSchemaSql();

[[nodiscard]] std::string CreateSchemaMigrationsLogEntrySql();

[[nodiscard]] std::string ReadSchemaMigrationsLogSql();
}
