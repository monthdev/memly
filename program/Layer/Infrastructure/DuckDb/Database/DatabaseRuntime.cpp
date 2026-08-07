#include "Layer/Infrastructure/DuckDb/Database/DatabaseRuntime.hpp"

#include <duckdb.hpp>

#include <memory>
#include <source_location>
#include <string>
#include <utility>

#include "Layer/Infrastructure/DuckDb/Database/PreparedStatement.hpp"
#include "Layer/Infrastructure/DuckDb/Database/ThrowOnDatabaseError.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

[[nodiscard]] auto DatabaseRuntime::PrepareStatement(const std::string& Sql, const std::source_location& SourceLocation) -> PreparedStatement {
    std::unique_ptr<duckdb::PreparedStatement> DuckDbPreparedStatement{ this->m_DatabaseConnection.Prepare(Sql) };
    ThrowOnPreparedStatementError(*DuckDbPreparedStatement, SourceLocation);
    return PreparedStatement{ std::move(DuckDbPreparedStatement) };
}

}
