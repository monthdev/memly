module;

#include <duckdb.hpp>

#include <string>

export module Memly.Database.DatabaseMigrator;

import Memly.Database.DatabaseRuntime;

export namespace Memly::Database {

class [[nodiscard]] DatabaseMigrator final {
private:
    duckdb::DuckDB m_Database;
    duckdb::Connection m_DatabaseConnection;

public:
    explicit DatabaseMigrator(const std::string&);

    explicit DatabaseMigrator(const DatabaseMigrator&) = delete;
    auto operator=(const DatabaseMigrator&) -> DatabaseMigrator& = delete;

    explicit DatabaseMigrator(DatabaseMigrator&&) = delete;
    auto operator=(DatabaseMigrator&&) -> DatabaseMigrator& = delete;

    ~DatabaseMigrator() noexcept = default;

    [[nodiscard]] auto ApplyMigrations() && -> DatabaseRuntime;
};

}
