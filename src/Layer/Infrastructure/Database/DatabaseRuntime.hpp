#pragma once

#include <duckdb.hpp>

#include <memory>
#include <source_location>
#include <string>

#include "Layer/Infrastructure/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/Database/PreparedStatementExecution.hpp"
#include "Layer/Infrastructure/Database/TransactionRunner.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::Database {

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

    [[nodiscard]] auto PrepareStatement(const std::string&, const std::source_location& = std::source_location::current()) -> PreparedStatement;

    [[nodiscard]] auto ExecutePreparedStatement(PreparedStatement&, const std::source_location& = std::source_location::current()) noexcept
        -> PreparedStatementExecution;
    [[nodiscard]] auto FetchNextDataChunk(duckdb::QueryResult&, const std::source_location& = std::source_location::current())
        -> duckdb::unique_ptr<duckdb::DataChunk>;

    [[nodiscard]] auto GetTransactionRunner() noexcept -> TransactionRunner&;

private:
    [[nodiscard]] auto ExecuteSql(const std::string&, const std::source_location& = std::source_location::current()) -> std::unique_ptr<duckdb::QueryResult>;

    void BootstrapDatabase();
    void ApplyMigrations();
    void SeedTableDefaults();
};

}
