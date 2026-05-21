#include "SeedSqlResource.hpp"

#include "SqlResourceBytes.hpp"

namespace Infrastructure::Sql {

[[nodiscard]] std::string CreateDefaultFsrs7SchedulerSql() {
    return ReadSqlResourceBytes(":/Sql/Seed/CreateDefaultFsrs7Scheduler.sql");
}

[[nodiscard]] std::string CreateDefaultFsrs7SettingsSql() {
    return ReadSqlResourceBytes(":/Sql/Seed/CreateDefaultFsrs7Settings.sql");
}

[[nodiscard]] std::string CreateDefaultDeckSettingsSql() {
    return ReadSqlResourceBytes(":/Sql/Seed/CreateDefaultDeckSettings.sql");
}
}
