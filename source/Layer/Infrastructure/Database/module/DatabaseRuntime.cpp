module;

#include <duckdb.hpp>

#include <memory>
#include <source_location>
#include <string>
#include <utility>

module Memly.Database.DatabaseRuntime;

import Memly.Database.PreparedStatement;

import Memly.Database.Internal.ThrowOnDatabaseError;

namespace Memly::Database {

DatabaseRuntime::DatabaseRuntime(duckdb::DatabaseInstance& DatabaseInstance, duckdb::Connection&& DatabaseConnection)
    : m_Database{ DatabaseInstance }
    , m_DatabaseConnection{ std::move(DatabaseConnection) } {
}

[[nodiscard]] auto DatabaseRuntime::PrepareStatement(const std::string& Sql, const std::source_location SourceLocation) -> PreparedStatement {
    std::unique_ptr<duckdb::PreparedStatement> DuckDbPreparedStatement{ this->m_DatabaseConnection.Prepare(Sql) };
    Internal::ThrowOnPreparedStatementError(*DuckDbPreparedStatement, SourceLocation);
    return PreparedStatement{ std::move(DuckDbPreparedStatement) };
}

}
