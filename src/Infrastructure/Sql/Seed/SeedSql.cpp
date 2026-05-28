#include "SeedSql.hpp"

#include "Infrastructure/Sql/SqlResourceBytes.hpp"

namespace Infrastructure::Sql::Seed {

[[nodiscard]] std::string CreateDefaultFsrs7SchedulerSql() {
    return ReadSqlResourceBytes(":/Sql/Seed/Statement/CreateDefaultFsrs7Scheduler.sql");
}

[[nodiscard]] std::string CreateDefaultFsrs7SettingsSql() {
    return ReadSqlResourceBytes(":/Sql/Seed/Statement/CreateDefaultFsrs7Settings.sql");
}

[[nodiscard]] std::string CreateDefaultDeckSettingsSql() {
    return ReadSqlResourceBytes(":/Sql/Seed/Statement/CreateDefaultDeckSettings.sql");
}
}
