#pragma once

#include <string>

namespace Infrastructure::Sql::Seed {

[[nodiscard]] auto CreateDefaultFsrs7SchedulerSql() -> std::string;

[[nodiscard]] auto CreateDefaultFsrs7SettingsSql() -> std::string;

[[nodiscard]] auto CreateDefaultDeckSettingsSql() -> std::string;
}
