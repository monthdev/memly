#pragma once

#include <duckdb.hpp>

#include <string>

#include "Infrastructure/Database/TransactionRunner.hpp"

namespace Infrastructure::Database {

class DatabaseRuntime final {
private:
    duckdb::DuckDB m_Database;
    duckdb::Connection m_DatabaseConnection;
    TransactionRunner m_TransactionRunner;

public:
    explicit DatabaseRuntime(const std::string& DatabaseFilePath)
        : m_Database{ DatabaseFilePath }
        , m_DatabaseConnection{ m_Database }
        , m_TransactionRunner{ m_DatabaseConnection } {
        BootstrapDatabase();
    }

    explicit DatabaseRuntime(const DatabaseRuntime&) = delete;
    explicit DatabaseRuntime(DatabaseRuntime&&) = delete;
    auto operator=(const DatabaseRuntime&) -> DatabaseRuntime& = delete;
    auto operator=(DatabaseRuntime&&) -> DatabaseRuntime& = delete;

    [[nodiscard]] auto GetDatabaseConnection() noexcept -> duckdb::Connection&;
    [[nodiscard]] auto GetTransactionRunner() noexcept -> TransactionRunner&;

private:
    void BootstrapDatabase();
    void ApplySchemaMigrations();
    void SeedTableDefaults();
};

}
