#pragma once

#include <string>

namespace Layer::Infrastructure::Sql::Migration {

[[nodiscard]] auto M00_MigrationsLogSql() -> std::string;

[[nodiscard]] auto M01_InitialSchemaSql() -> std::string;

[[nodiscard]] auto CreateMigrationsLogEntrySql() -> std::string;

[[nodiscard]] auto ReadMigrationsLogSql() -> std::string;
}
