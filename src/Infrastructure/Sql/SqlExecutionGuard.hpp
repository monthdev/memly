#pragma once

#include <string_view>

namespace duckdb {
class PreparedStatement;
class QueryResult;
}

namespace Infrastructure::Sql {

void ThrowUnconditionallyOnQueryResultError(duckdb::QueryResult&);

void ThrowUnconditionallyOnPreparedStatementError(duckdb::PreparedStatement&);

void ThrowUnconditionallyOnMutationNoOp(duckdb::QueryResult&, const std::string_view);

}
