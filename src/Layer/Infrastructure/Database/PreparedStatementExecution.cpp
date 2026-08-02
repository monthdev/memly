#include "Layer/Infrastructure/Database/PreparedStatementExecution.hpp"

#include <duckdb.hpp>

#include <initializer_list>
#include <memory>
#include <string_view>
#include <utility>

#include "Layer/Infrastructure/Database/QueryResultDecoder.hpp"
#include "Support/Runtime/Exception/ThrowMemlyException.hpp"

namespace Layer::Infrastructure::Database {

[[nodiscard]] auto PreparedStatementExecution::WithoutParameters() && -> QueryResultDecoder {
    return QueryResultDecoder{ Execute(duckdb::vector<duckdb::Value>{}) };
}

// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
[[nodiscard]] auto PreparedStatementExecution::Execute(duckdb::vector<duckdb::Value>&& DuckDbValueVector) -> QueryResultDecoder {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DuckDbPreparedStatement.Execute(DuckDbValueVector, true) };
    if (QueryResult->HasError()) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ QueryResult->GetError() }, m_SourceLocation);
    }
    return QueryResultDecoder{ std::move(QueryResult), m_SourceLocation };
}

}
