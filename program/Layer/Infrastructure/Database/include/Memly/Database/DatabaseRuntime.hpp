#pragma once

#include <duckdb.hpp>

#include <source_location>
#include <string>

#include "Memly/Database/PreparedStatement.hpp"
#include "Memly/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Database {

class [[nodiscard]] DatabaseRuntime final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    duckdb::DuckDB m_Database;
    duckdb::Connection m_DatabaseConnection;

public:
    explicit DatabaseRuntime(duckdb::DatabaseInstance&, duckdb::Connection&&);

    [[nodiscard]] auto PrepareStatement(const std::string&, const std::source_location& = std::source_location::current()) -> PreparedStatement;
};

}
