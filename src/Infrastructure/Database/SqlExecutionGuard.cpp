#include "Infrastructure/Database/SqlExecutionGuard.hpp"

#include <duckdb.hpp>

#include "Support/Runtime/ThrowMemlyException.hpp"

namespace Infrastructure::Database {

void ThrowOnQueryResultError(duckdb::QueryResult& QueryResult, const std::source_location& SourceLocation) {
    if (QueryResult.HasError()) {
        Support::Runtime::ThrowMemlyException(QueryResult.GetError(), SourceLocation);
    }
}

void ThrowOnPreparedStatementError(duckdb::PreparedStatement& PreparedStatement, const std::source_location& SourceLocation) {
    if (PreparedStatement.HasError()) {
        Support::Runtime::ThrowMemlyException(PreparedStatement.GetError(), SourceLocation);
    }
}

void ThrowOnMutationNoOp(duckdb::QueryResult& QueryResult, const std::string_view ErrorMessage, const std::source_location& SourceLocation) {
    if (QueryResult.begin() not_eq QueryResult.end()) {
        return;
    }
    Support::Runtime::ThrowMemlyException(ErrorMessage, SourceLocation);
}

}
