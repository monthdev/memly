#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

#include <duckdb.hpp>

#include "Runtime/Crash.hpp"

namespace Infrastructure::Sql {

void ThrowOnQueryResultError(duckdb::QueryResult& QueryResult) {
    if (QueryResult.HasError()) {
        Runtime::ThrowError(QueryResult.GetError());
    }
}

void ThrowOnPreparedStatementError(duckdb::PreparedStatement& PreparedStatement) {
    if (PreparedStatement.HasError()) {
        Runtime::ThrowError(PreparedStatement.GetError());
    }
}

void ThrowOnMutationNoOp(duckdb::QueryResult& QueryResult, const std::string_view ErrorMessage) {
    if (QueryResult.begin() not_eq QueryResult.end()) {
        return;
    }
    Runtime::ThrowError(ErrorMessage);
}

}
