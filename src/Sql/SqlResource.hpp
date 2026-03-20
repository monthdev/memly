#pragma once

#include <string>

namespace Sql {

[[nodiscard]] std::string M00_MigrationsTableSql();

[[nodiscard]] std::string M01_InitialSchemaSql();

[[nodiscard]] std::string CreateMigrationSql();

[[nodiscard]] std::string ReadMigrationsTableSql();

[[nodiscard]] std::string CreateDeckSql();

[[nodiscard]] std::string ReadDecksTableSql();

[[nodiscard]] std::string UpdateDeckSql();

[[nodiscard]] std::string DeleteDeckSql();
}
