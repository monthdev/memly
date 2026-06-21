#pragma once

#include <duckdb.hpp>

#include <string>
#include <string_view>

#include "Infrastructure/Database/TransactionRunner.hpp"

namespace Infrastructure::Database {

class DatabaseRuntime final {
public:
    explicit DatabaseRuntime(const std::string_view DatabaseFilePathStringView)
        : m_Database{ std::string{ DatabaseFilePathStringView } }
        , m_DatabaseConnection{ m_Database }
        , m_TransactionRunner{ m_DatabaseConnection } {
        BootstrapDatabase();
    }

    ~DatabaseRuntime() noexcept = default;
    DatabaseRuntime(const DatabaseRuntime&) = delete;
    DatabaseRuntime(DatabaseRuntime&&) = delete;
    DatabaseRuntime& operator=(const DatabaseRuntime&) = delete;
    DatabaseRuntime& operator=(DatabaseRuntime&&) = delete;

    [[nodiscard]] duckdb::Connection& GetDatabaseConnection() noexcept {
        return m_DatabaseConnection;
    }

    [[nodiscard]] TransactionRunner& GetTransactionRunner() noexcept {
        return m_TransactionRunner;
    }

private:
    void BootstrapDatabase();
    void ApplySchemaMigrations();
    void SeedTableDefaults();

    duckdb::DuckDB m_Database;
    duckdb::Connection m_DatabaseConnection;
    TransactionRunner m_TransactionRunner;
};

}
