#pragma once

#include <duckdb.hpp>

#include <string>

#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {
class DatabaseRuntime;

class [[nodiscard]] DatabaseMigrator final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    duckdb::DuckDB m_Database;
    duckdb::Connection m_DatabaseConnection;

public:
    explicit DatabaseMigrator(const std::string& DatabaseFilePath)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_Database{ DatabaseFilePath }
        , m_DatabaseConnection{ this->m_Database } {
    }

    [[nodiscard]] auto ApplyMigrations() && -> DatabaseRuntime;
};

}
