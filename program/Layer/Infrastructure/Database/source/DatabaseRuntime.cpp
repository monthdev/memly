#include "Memly/Database/DatabaseRuntime.hpp"

#include <duckdb.hpp>

#include <memory>
#include <source_location>
#include <string>
#include <utility>

#include "Memly/Database/PreparedStatement.hpp"
#include "_ThrowOnDatabaseError.hpp"

namespace Memly::Database {

DatabaseRuntime::DatabaseRuntime(duckdb::DatabaseInstance& DatabaseInstance, duckdb::Connection&& DatabaseConnection)
    : m_Database{ DatabaseInstance }
    , m_DatabaseConnection{ std::move(DatabaseConnection) } {
}

[[nodiscard]] auto DatabaseRuntime::PrepareStatement(const std::string& Sql, const std::source_location& SourceLocation) -> PreparedStatement {
    std::unique_ptr<duckdb::PreparedStatement> DuckDbPreparedStatement{ this->m_DatabaseConnection.Prepare(Sql) };
    i_ThrowOnPreparedStatementError(*DuckDbPreparedStatement, SourceLocation);
    return PreparedStatement{ std::move(DuckDbPreparedStatement) };
}

}
