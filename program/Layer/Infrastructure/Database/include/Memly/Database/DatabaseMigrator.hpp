#pragma once

#include <duckdb.hpp>

#include <string>

#include "Memly/Database/DatabaseRuntime.hpp"
#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Database {

class [[nodiscard]] DatabaseMigrator final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    duckdb::DuckDB m_Database;
    duckdb::Connection m_DatabaseConnection;

public:
    explicit DatabaseMigrator(const std::string&);

    [[nodiscard]] auto ApplyMigrations() && -> DatabaseRuntime;
};

}
