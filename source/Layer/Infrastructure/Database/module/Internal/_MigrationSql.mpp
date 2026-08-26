module;

#include <string>

export module Memly.Database.Internal.MigrationSql;

export namespace Memly::Database::Internal {

[[nodiscard]] auto CreateMigrationsLogSql() -> std::string;

[[nodiscard]] auto M01CreateSchemaSql() -> std::string;

[[nodiscard]] auto M02SeedTableDefaultsSql() -> std::string;

[[nodiscard]] auto SelectAppliedMigrationVersionsSql() -> std::string;

[[nodiscard]] auto InsertMigrationLogEntrySql() -> std::string;
}
