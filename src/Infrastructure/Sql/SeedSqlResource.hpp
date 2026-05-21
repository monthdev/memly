#pragma once

#include <string>

namespace Infrastructure::Sql {

[[nodiscard]] std::string CreateDefaultFsrs7SchedulerSql();

[[nodiscard]] std::string CreateDefaultFsrs7SettingsSql();

[[nodiscard]] std::string CreateDefaultDeckSettingsSql();
}
