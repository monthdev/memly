#include "Memly/Database/PreparedStatement.hpp"

#include <duckdb.hpp>

#include <memory>
#include <source_location>
#include <utility>

#include "Memly/Database/PreparedStatementExecution.hpp"

namespace Layer::Infrastructure::Database {

PreparedStatement::PreparedStatement(std::unique_ptr<duckdb::PreparedStatement>&& DuckDbPreparedStatement) noexcept
    : m_DuckDbPreparedStatement{ std::move(DuckDbPreparedStatement) } {
}

[[nodiscard]] auto PreparedStatement::Execute(const std::source_location& SourceLocation) noexcept -> PreparedStatementExecution {
    return PreparedStatementExecution{ *this->m_DuckDbPreparedStatement, SourceLocation };
}

}
