#pragma once

#include <source_location>

namespace duckdb {
class PreparedStatement;
class QueryResult;
}

namespace Layer::Infrastructure::Database {

void ThrowOnPreparedStatementError(duckdb::PreparedStatement&, const std::source_location& = std::source_location::current());

void ThrowOnQueryResultError(duckdb::QueryResult&, const std::source_location& = std::source_location::current());

}
