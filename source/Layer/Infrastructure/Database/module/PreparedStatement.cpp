module;

#include <duckdb.hpp>

#include <memory>
#include <source_location>
#include <utility>

module Memly.Database.PreparedStatement;

import Memly.Database.PreparedStatementExecution;

namespace Memly::Database {

PreparedStatement::PreparedStatement(
    std::unique_ptr<duckdb::PreparedStatement>&& DuckDbPreparedStatement
) noexcept : m_DuckDbPreparedStatement{ std::move(DuckDbPreparedStatement) } {
}

[[nodiscard]] PreparedStatementExecution
PreparedStatement::Execute(
    const std::source_location SourceLocation
) & noexcept {
    return PreparedStatementExecution{ *this->m_DuckDbPreparedStatement,
        SourceLocation };
}

}
