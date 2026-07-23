#include "Infrastructure/Database/SqlExecutionGuard.hpp"

#include <duckdb.hpp>

#include <source_location>

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

}
