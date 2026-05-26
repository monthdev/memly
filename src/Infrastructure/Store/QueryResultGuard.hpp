#pragma once

#include <memory>
#include <string_view>

namespace duckdb {
class QueryResult;
}

namespace Infrastructure::Store {

void ThrowOnQueryResultError(const std::unique_ptr<duckdb::QueryResult>&);

void ThrowOnMutationNoOp(const std::unique_ptr<duckdb::QueryResult>&, const std::string_view);

}
