#pragma once

#include <string>

namespace Layer::Infrastructure::Sql::Migration {

[[nodiscard]] auto M00_CreateMigrationsLogSql() -> std::string;

[[nodiscard]] auto M01_CreateSchemaSql() -> std::string;

[[nodiscard]] auto CreateMigrationsLogEntrySql() -> std::string;

[[nodiscard]] auto ReadMigrationsLogSql() -> std::string;
}
