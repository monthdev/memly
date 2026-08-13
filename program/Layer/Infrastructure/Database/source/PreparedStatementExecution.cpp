#include "Memly/Database/PreparedStatementExecution.hpp"

#include <duckdb.hpp>

#include <memory>
#include <source_location>
#include <utility>

#include "Memly/Database/QueryResultDecoder.hpp"
#include "ThrowOnDatabaseError.hpp"

namespace Layer::Infrastructure::Database {

PreparedStatementExecution::PreparedStatementExecution(duckdb::PreparedStatement& DuckDbPreparedStatement, const std::source_location& SourceLocation) noexcept
    : m_DuckDbPreparedStatement{ DuckDbPreparedStatement }
    , m_SourceLocation{ SourceLocation } {
}

[[nodiscard]] auto PreparedStatementExecution::WithoutParameters() && -> QueryResultDecoder {
    return QueryResultDecoder{ this->Execute(duckdb::vector<duckdb::Value>{}) };
}

// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
[[nodiscard]] auto PreparedStatementExecution::Execute(duckdb::vector<duckdb::Value>&& DuckDbValueVector) -> QueryResultDecoder {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ this->m_DuckDbPreparedStatement.Execute(DuckDbValueVector, true) };
    ThrowOnQueryResultError(*QueryResult, this->m_SourceLocation);
    return QueryResultDecoder{ std::move(QueryResult), this->m_SourceLocation };
}

}
