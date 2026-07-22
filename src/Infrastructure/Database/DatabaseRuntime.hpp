#pragma once

#include <duckdb.hpp>

#include <string>

#include "Infrastructure/Database/TransactionRunner.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Infrastructure::Database {

class DatabaseRuntime final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    duckdb::DuckDB m_Database;
    duckdb::Connection m_DatabaseConnection;
    TransactionRunner m_TransactionRunner;

public:
    explicit DatabaseRuntime(const std::string& DatabaseFilePath)
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_Database{ DatabaseFilePath }
        , m_DatabaseConnection{ m_Database }
        , m_TransactionRunner{ m_DatabaseConnection } {
        BootstrapDatabase();
    }

    [[nodiscard]] auto GetDatabaseConnection() noexcept -> duckdb::Connection&;
    [[nodiscard]] auto GetTransactionRunner() noexcept -> TransactionRunner&;

private:
    void BootstrapDatabase();
    void ApplySchemaMigrations();
    void SeedTableDefaults();
};

}
