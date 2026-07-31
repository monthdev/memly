#include "Layer/Infrastructure/Database/PreparedStatementExecution.hpp"

#include <duckdb.hpp>

#include <initializer_list>
#include <memory>
#include <string_view>

#include "Support/Runtime/Exception/ThrowMemlyException.hpp"

namespace Layer::Infrastructure::Database {

[[nodiscard]] auto PreparedStatementExecution::WithoutParameters() && -> std::unique_ptr<duckdb::QueryResult> {
    return Execute(duckdb::vector<duckdb::Value>{});
}

// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
[[nodiscard]] auto PreparedStatementExecution::Execute(duckdb::vector<duckdb::Value>&& DuckDbValueVector) -> std::unique_ptr<duckdb::QueryResult> {
    std::unique_ptr<duckdb::QueryResult> QueryResult{ m_DuckDbPreparedStatement.Execute(DuckDbValueVector, true) };
    if (QueryResult->HasError()) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ QueryResult->GetError() }, m_SourceLocation);
    }
    return QueryResult;
}

}
