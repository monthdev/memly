#pragma once

#include <string>

namespace Memly::Database {

[[nodiscard]] auto CreateMigrationsLogSql() -> std::string;

[[nodiscard]] auto M01_CreateSchemaSql() -> std::string;

[[nodiscard]] auto M02_SeedTableDefaultsSql() -> std::string;

[[nodiscard]] auto SelectAppliedMigrationVersionsSql() -> std::string;

[[nodiscard]] auto InsertMigrationLogEntrySql() -> std::string;
}
