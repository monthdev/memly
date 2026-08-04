#pragma once

#include <string>

namespace Layer::Infrastructure::DuckDb::Database::Sql::Migration {

[[nodiscard]] auto CreateMigrationsLogSql() -> std::string;

[[nodiscard]] auto M01_CreateSchemaSql() -> std::string;

[[nodiscard]] auto SelectAppliedMigrationVersionsSql() -> std::string;

[[nodiscard]] auto InsertMigrationLogEntrySql() -> std::string;
}
