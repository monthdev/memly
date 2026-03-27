#pragma once

#include <string>

namespace Sql {

[[nodiscard]] std::string M00_MigrationsTableSql();

[[nodiscard]] std::string M01_InitialSchemaSql();

[[nodiscard]] std::string CreateMigrationSql();

[[nodiscard]] std::string ReadMigrationsTableSql();

[[nodiscard]] std::string CreateDefaultFsrsConfigurationSql();

[[nodiscard]] std::string CreateDeckSql();

[[nodiscard]] std::string ReadDeckListViewSql();

[[nodiscard]] std::string UpdateDeckNameSql();

[[nodiscard]] std::string DeleteDeckSql();

}
