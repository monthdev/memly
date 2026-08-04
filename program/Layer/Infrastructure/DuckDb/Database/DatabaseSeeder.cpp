#include "Layer/Infrastructure/DuckDb/Database/DatabaseSeeder.hpp"

#include <duckdb.hpp>

#include "Layer/Infrastructure/DuckDb/Database/Sql/Seed/SeedSql.hpp"
#include "Layer/Infrastructure/DuckDb/Database/ThrowOnDatabaseError.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

void DatabaseSeeder::SeedTableDefaults() {
    ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Sql::Seed::InsertDefaultFsrs7SchedulerSql()));
    ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Sql::Seed::InsertDefaultFsrs7SettingsSql()));
    ThrowOnQueryResultError(*this->m_DatabaseConnection.Query(Sql::Seed::InsertDefaultDeckSettingsSql()));
}

}
