#include "SeedSql.hpp"

#include <string>

#include "Support/Runtime/QtApp/QtAppResourceBytes.hpp"

namespace Layer::Infrastructure::DuckDb::Database::Sql::Seed {

[[nodiscard]] auto InsertDefaultFsrs7SchedulerSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/Insert/InsertDefaultFsrs7Scheduler.sql") };
}

[[nodiscard]] auto InsertDefaultFsrs7SettingsSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/Insert/InsertDefaultFsrs7Settings.sql") };
}

[[nodiscard]] auto InsertDefaultDeckSettingsSql() -> std::string {
    return std::string{ Support::Runtime::QtApp::ReadQtAppResourceBytes(":/Sql/Seed/Insert/InsertDefaultDeckSettings.sql") };
}
}
