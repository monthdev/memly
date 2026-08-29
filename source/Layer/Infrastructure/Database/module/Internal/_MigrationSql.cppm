module;

#include <string>

export module Memly.Database.Internal.MigrationSql;

export namespace Memly::Database::Internal {

[[nodiscard]] std::string
CreateMigrationsLogSql();

[[nodiscard]] std::string
M01CreateSchemaSql();

[[nodiscard]] std::string
M02SeedTableDefaultsSql();

[[nodiscard]] std::string
SelectAppliedMigrationVersionsSql();

[[nodiscard]] std::string
InsertMigrationLogEntrySql();
}
