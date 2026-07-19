#include "SeedSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::Seed {

[[nodiscard]] auto CreateDefaultFsrs7SchedulerSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/Statement/CreateDefaultFsrs7Scheduler.sql");
}

[[nodiscard]] auto CreateDefaultFsrs7SettingsSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/Statement/CreateDefaultFsrs7Settings.sql");
}

[[nodiscard]] auto CreateDefaultDeckSettingsSql() -> std::string {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/Statement/CreateDefaultDeckSettings.sql");
}
}
