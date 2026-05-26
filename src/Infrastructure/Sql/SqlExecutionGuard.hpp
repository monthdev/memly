#pragma once

#include <string_view>

namespace duckdb {
class PreparedStatement;
class QueryResult;
}

namespace Infrastructure::Sql {

void ThrowOnQueryResultError(duckdb::QueryResult&);

void ThrowOnPreparedStatementError(duckdb::PreparedStatement&);

void ThrowOnMutationNoOp(duckdb::QueryResult&, const std::string_view);

}
