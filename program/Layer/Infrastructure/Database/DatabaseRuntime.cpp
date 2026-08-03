#include "Layer/Infrastructure/Database/DatabaseRuntime.hpp"

#include <duckdb.hpp>

#include <memory>
#include <source_location>
#include <string>
#include <utility>

#include "Layer/Infrastructure/Database/DatabaseMigrator.hpp"
#include "Layer/Infrastructure/Database/DatabaseSeeder.hpp"
#include "Layer/Infrastructure/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/Database/ThrowOnDatabaseError.hpp"
#include "Layer/Infrastructure/Database/TransactionRunner.hpp"

namespace Layer::Infrastructure::Database {

[[nodiscard]] auto DatabaseRuntime::PrepareStatement(const std::string& Sql, const std::source_location& SourceLocation) -> PreparedStatement {
    std::unique_ptr<duckdb::PreparedStatement> DuckDbPreparedStatement{ this->m_DatabaseConnection.Prepare(Sql) };
    ThrowOnPreparedStatementError(*DuckDbPreparedStatement, SourceLocation);
    return PreparedStatement{ std::move(DuckDbPreparedStatement) };
}

[[nodiscard]] auto DatabaseRuntime::GetTransactionRunner() noexcept -> TransactionRunner& {
    return this->m_TransactionRunner;
}

void DatabaseRuntime::BootstrapDatabase() {
    this->m_TransactionRunner.TransactionWrapper([this]() -> void {
        DatabaseMigrator{ this->m_DatabaseConnection }.ApplyMigrations();
        DatabaseSeeder{ this->m_DatabaseConnection }.SeedTableDefaults();
    });
}

}
