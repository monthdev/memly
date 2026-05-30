#include "Infrastructure/Sql/SqlExecutionGuard.hpp"

#include <duckdb.hpp>

#include "Runtime/Crash.hpp"

namespace Infrastructure::Sql {

void CrashOnQueryResultError(duckdb::QueryResult& QueryResult, const std::source_location& SourceLocation) {
    if (QueryResult.HasError()) {
        Runtime::Crash(QueryResult.GetError(), SourceLocation);
    }
}

void CrashOnPreparedStatementError(duckdb::PreparedStatement& PreparedStatement, const std::source_location& SourceLocation) {
    if (PreparedStatement.HasError()) {
        Runtime::Crash(PreparedStatement.GetError(), SourceLocation);
    }
}

void CrashOnMutationNoOp(duckdb::QueryResult& QueryResult, const std::string_view ErrorMessage, const std::source_location& SourceLocation) {
    if (QueryResult.begin() not_eq QueryResult.end()) {
        return;
    }
    Runtime::Crash(ErrorMessage, SourceLocation);
}

}
