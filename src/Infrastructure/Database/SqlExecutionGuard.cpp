#include "Infrastructure/Database/SqlExecutionGuard.hpp"

#include <duckdb.hpp>

#include "Runtime/Crash.hpp"

namespace Infrastructure::Database {

void ThrowOnQueryResultError(duckdb::QueryResult& QueryResult, const std::source_location& SourceLocation) {
    if (QueryResult.HasError()) {
        Runtime::ThrowError(QueryResult.GetError(), SourceLocation);
    }
}

void ThrowOnPreparedStatementError(duckdb::PreparedStatement& PreparedStatement, const std::source_location& SourceLocation) {
    if (PreparedStatement.HasError()) {
        Runtime::ThrowError(PreparedStatement.GetError(), SourceLocation);
    }
}

void ThrowOnMutationNoOp(duckdb::QueryResult& QueryResult, const std::string_view ErrorMessage, const std::source_location& SourceLocation) {
    if (QueryResult.begin() not_eq QueryResult.end()) {
        return;
    }
    Runtime::ThrowError(ErrorMessage, SourceLocation);
}

}
