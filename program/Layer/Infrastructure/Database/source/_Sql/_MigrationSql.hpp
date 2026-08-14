#pragma once

#include <string>

namespace Memly::Database {

[[nodiscard]] auto i_CreateMigrationsLogSql() -> std::string;

[[nodiscard]] auto i_M01_CreateSchemaSql() -> std::string;

[[nodiscard]] auto i_M02_SeedTableDefaultsSql() -> std::string;

[[nodiscard]] auto i_SelectAppliedMigrationVersionsSql() -> std::string;

[[nodiscard]] auto i_InsertMigrationLogEntrySql() -> std::string;
}
