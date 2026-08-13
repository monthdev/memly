#pragma once

#include <string>

namespace Layer::Infrastructure::Database::Sql {

[[nodiscard]] auto CreateMigrationsLogSql() -> std::string;

[[nodiscard]] auto M01_CreateSchemaSql() -> std::string;

[[nodiscard]] auto M02_SeedTableDefaultsSql() -> std::string;

[[nodiscard]] auto SelectAppliedMigrationVersionsSql() -> std::string;

[[nodiscard]] auto InsertMigrationLogEntrySql() -> std::string;
}
