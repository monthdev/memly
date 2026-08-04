#include "SeedSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::Persistence::Sql::Seed {

[[nodiscard]] auto CreateDefaultFsrs7SchedulerSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/CreateDefaultFsrs7Scheduler.sql") };
}

[[nodiscard]] auto CreateDefaultFsrs7SettingsSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/CreateDefaultFsrs7Settings.sql") };
}

[[nodiscard]] auto CreateDefaultDeckSettingsSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/CreateDefaultDeckSettings.sql") };
}
}
