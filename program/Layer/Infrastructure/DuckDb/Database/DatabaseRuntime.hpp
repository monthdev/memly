#pragma once

#include <duckdb.hpp>

#include <source_location>
#include <string>
#include <utility>

#include "Layer/Infrastructure/DuckDb/Database/PreparedStatement.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {
class DatabaseMigrator;

class [[nodiscard]] DatabaseRuntime final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    friend class DatabaseMigrator;

private:
    duckdb::DuckDB m_Database;
    duckdb::Connection m_DatabaseConnection;

    explicit DatabaseRuntime(duckdb::DatabaseInstance& DatabaseInstance, duckdb::Connection&& DatabaseConnection)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_Database{ DatabaseInstance }
        , m_DatabaseConnection{ std::move(DatabaseConnection) } {
    }

public:
    [[nodiscard]] auto PrepareStatement(const std::string&, const std::source_location& = std::source_location::current()) -> PreparedStatement;
};

}
