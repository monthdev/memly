#pragma once

#include <string>

namespace Infrastructure::Sql {

[[nodiscard]] std::string M00_MigrationsTableSql();

[[nodiscard]] std::string M01_InitialSchemaSql();

[[nodiscard]] std::string CreateMigrationSql();

[[nodiscard]] std::string ReadMigrationsTableSql();

[[nodiscard]] std::string CreateDefaultFsrs7SchedulerSql();

[[nodiscard]] std::string CreateDefaultFsrs7SettingsSql();

[[nodiscard]] std::string CreateDefaultDeckSettingsSql();

[[nodiscard]] std::string CreateRootDeckSql();

[[nodiscard]] std::string CreateChildDeckSql();

[[nodiscard]] std::string ReadDeckHierarchyViewSnapshotSql();

[[nodiscard]] std::string MoveDeckSql();

[[nodiscard]] std::string UpdateDeckNameSql();

[[nodiscard]] std::string DeleteDeckCardReviewsSql();

[[nodiscard]] std::string DeleteDeckCardsSql();

[[nodiscard]] std::string DeleteDeckSql();

}
