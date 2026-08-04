#pragma once

#include <string>

namespace Layer::Infrastructure::DuckDb::Database::Sql::Seed {

[[nodiscard]] auto InsertDefaultFsrs7SchedulerSql() -> std::string;

[[nodiscard]] auto InsertDefaultFsrs7SettingsSql() -> std::string;

[[nodiscard]] auto InsertDefaultDeckSettingsSql() -> std::string;
}
