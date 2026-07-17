#include "SeedSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Infrastructure::Sql::Seed {

[[nodiscard]] std::string CreateDefaultFsrs7SchedulerSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/Statement/CreateDefaultFsrs7Scheduler.sql");
}

[[nodiscard]] std::string CreateDefaultFsrs7SettingsSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/Statement/CreateDefaultFsrs7Settings.sql");
}

[[nodiscard]] std::string CreateDefaultDeckSettingsSql() {
    return Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/Statement/CreateDefaultDeckSettings.sql");
}
}
