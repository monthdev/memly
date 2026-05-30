#pragma once

#include <source_location>
#include <string_view>

namespace duckdb {
class PreparedStatement;
class QueryResult;
}

namespace Infrastructure::Sql {

void CrashOnQueryResultError(duckdb::QueryResult&, const std::source_location& = std::source_location::current());

void CrashOnPreparedStatementError(duckdb::PreparedStatement&, const std::source_location& = std::source_location::current());

void CrashOnMutationNoOp(duckdb::QueryResult&, const std::string_view, const std::source_location& = std::source_location::current());

}
