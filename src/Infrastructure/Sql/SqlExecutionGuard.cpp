#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

#include <duckdb.hpp>

#include "Runtime/Crash.hpp"

namespace Infrastructure::Sql {

void ThrowUnconditionallyOnQueryResultError(duckdb::QueryResult& QueryResult) {
    if (QueryResult.HasError()) {
        Runtime::ThrowError(QueryResult.GetError());
    }
}

void ThrowUnconditionallyOnPreparedStatementError(duckdb::PreparedStatement& PreparedStatement) {
    if (PreparedStatement.HasError()) {
        Runtime::ThrowError(PreparedStatement.GetError());
    }
}

void ThrowUnconditionallyOnMutationNoOp(duckdb::QueryResult& QueryResult, const std::string_view ErrorMessage) {
    if (QueryResult.begin() not_eq QueryResult.end()) {
        return;
    }
    Runtime::ThrowError(ErrorMessage);
}

}
