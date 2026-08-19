#pragma once

#include <duckdb.hpp>

#include <source_location>
#include <string>

#include "Memly/Database/PreparedStatement.hpp"

namespace Memly::Database {

class [[nodiscard]] DatabaseRuntime final {
private:
    duckdb::DuckDB m_Database;
    duckdb::Connection m_DatabaseConnection;

public:
    explicit DatabaseRuntime(duckdb::DatabaseInstance&, duckdb::Connection&&);

    explicit DatabaseRuntime(const DatabaseRuntime&) = delete;
    auto operator=(const DatabaseRuntime&) -> DatabaseRuntime& = delete;

    explicit DatabaseRuntime(DatabaseRuntime&&) = delete;
    auto operator=(DatabaseRuntime&&) -> DatabaseRuntime& = delete;

    ~DatabaseRuntime() noexcept = default;

    [[nodiscard]] auto PrepareStatement(const std::string&, std::source_location = std::source_location::current()) -> PreparedStatement;
};

}
