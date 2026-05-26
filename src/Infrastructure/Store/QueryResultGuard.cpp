#include "Infrastructure/Store/QueryResultGuard.hpp"

#include <duckdb.hpp>

#include "Runtime/Crash.hpp"

namespace Infrastructure::Store {

void ThrowOnQueryResultError(const std::unique_ptr<duckdb::QueryResult>& QueryResult) {
    if (QueryResult->HasError()) {
        Runtime::ThrowError(QueryResult->GetError());
    }
}

void ThrowOnMutationNoOp(const std::unique_ptr<duckdb::QueryResult>& QueryResult, const std::string_view ErrorMessage) {
    if (QueryResult->begin() not_eq QueryResult->end()) {
        return;
    }
    Runtime::ThrowError(ErrorMessage);
}

}
