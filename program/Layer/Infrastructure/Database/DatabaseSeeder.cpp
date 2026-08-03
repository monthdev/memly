#include "Layer/Infrastructure/Database/DatabaseSeeder.hpp"

#include <duckdb.hpp>

#include "Layer/Infrastructure/Database/ThrowOnDatabaseError.hpp"
#include "Layer/Infrastructure/Sql/Seed/SeedSql.hpp"

namespace Layer::Infrastructure::Database {

void DatabaseSeeder::SeedTableDefaults() {
    ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Sql::Seed::CreateDefaultFsrs7SchedulerSql()));
    ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Sql::Seed::CreateDefaultFsrs7SettingsSql()));
    ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Sql::Seed::CreateDefaultDeckSettingsSql()));
}

}
