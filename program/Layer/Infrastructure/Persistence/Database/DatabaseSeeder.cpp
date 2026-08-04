#include "Layer/Infrastructure/Persistence/Database/DatabaseSeeder.hpp"

#include <duckdb.hpp>

#include "Layer/Infrastructure/Persistence/Database/ThrowOnDatabaseError.hpp"
#include "Layer/Infrastructure/Persistence/Sql/Seed/SeedSql.hpp"

namespace Layer::Infrastructure::Persistence::Database {

void DatabaseSeeder::SeedTableDefaults() {
    ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Sql::Seed::CreateDefaultFsrs7SchedulerSql()));
    ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Sql::Seed::CreateDefaultFsrs7SettingsSql()));
    ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Sql::Seed::CreateDefaultDeckSettingsSql()));
}

}
