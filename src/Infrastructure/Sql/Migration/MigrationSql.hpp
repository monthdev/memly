#pragma once

#include <string>

namespace Infrastructure::Sql::Migration {

[[nodiscard]] auto M00_SchemaMigrationsLogSql() -> std::string;

[[nodiscard]] auto M01_InitialSchemaSql() -> std::string;

[[nodiscard]] auto CreateSchemaMigrationsLogEntrySql() -> std::string;

[[nodiscard]] auto ReadSchemaMigrationsLogSql() -> std::string;
}
