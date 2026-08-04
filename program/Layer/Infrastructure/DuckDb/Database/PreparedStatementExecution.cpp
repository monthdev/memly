#include "Layer/Infrastructure/DuckDb/Database/PreparedStatementExecution.hpp"

#include <duckdb.hpp>

#include <memory>
#include <utility>

#include "Layer/Infrastructure/DuckDb/Database/QueryResultDecoder.hpp"
#include "Layer/Infrastructure/DuckDb/Database/ThrowOnDatabaseError.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

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
