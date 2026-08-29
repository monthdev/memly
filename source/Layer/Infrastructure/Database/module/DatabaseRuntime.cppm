module;

#include <duckdb.hpp>

#include <source_location>
#include <string>

export module Memly.Database.DatabaseRuntime;

import Memly.Database.PreparedStatement;

export namespace Memly::Database {

class [[nodiscard]] DatabaseRuntime final {
private:
    duckdb::DuckDB m_Database;
    duckdb::Connection m_DatabaseConnection;

public:
    explicit DatabaseRuntime(duckdb::DatabaseInstance&, duckdb::Connection&&);

    explicit DatabaseRuntime(const DatabaseRuntime&) = delete;
    DatabaseRuntime&
    operator=(const DatabaseRuntime&) = delete;

    explicit DatabaseRuntime(DatabaseRuntime&&) = delete;
    DatabaseRuntime&
    operator=(DatabaseRuntime&&) = delete;

    ~DatabaseRuntime() noexcept = default;

    [[nodiscard]] PreparedStatement
    PrepareStatement(
        const std::string&,
        std::source_location = std::source_location::current()
    );
};

}
