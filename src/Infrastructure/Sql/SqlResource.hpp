#pragma once

#include <string>

namespace Infrastructure::Sql {

[[nodiscard]] const std::string& M00_MigrationsTableSql();

[[nodiscard]] const std::string& M01_InitialSchemaSql();

[[nodiscard]] const std::string& CreateMigrationSql();

[[nodiscard]] const std::string& ReadMigrationsTableSql();

[[nodiscard]] const std::string& CreateDefaultFsrs7SchedulerSql();

[[nodiscard]] const std::string& CreateDefaultFsrs7SettingsSql();

[[nodiscard]] const std::string& CreateDefaultDeckSettingsSql();

[[nodiscard]] const std::string& CreateRootDeckSql();

[[nodiscard]] const std::string& CreateChildDeckSql();

[[nodiscard]] const std::string& ReadDeckTreeSnapshotSql();

[[nodiscard]] const std::string& ReadNextLibraryRefreshAtMillisecondsSinceEpochSql();

[[nodiscard]] const std::string& ValidateDeckExistsSql();

[[nodiscard]] const std::string& ValidateParentDeckExistsSql();

[[nodiscard]] const std::string& MoveDeckSql();

[[nodiscard]] const std::string& RenameDeckSql();

[[nodiscard]] const std::string& DeleteDeckCardReviewsSql();

[[nodiscard]] const std::string& DeleteDeckCardsSql();

[[nodiscard]] const std::string& DeleteDeckSql();
}
