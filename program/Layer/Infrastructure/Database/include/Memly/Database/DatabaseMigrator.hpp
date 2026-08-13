#pragma once

#include <duckdb.hpp>

#include <string>

#include "Memly/Database/DatabaseRuntime.hpp"

namespace Layer::Infrastructure::Database {

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
