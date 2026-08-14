#pragma once

#include <duckdb.hpp>

#include <source_location>

namespace Memly::Database {

void i_ThrowOnPreparedStatementError(duckdb::PreparedStatement&, const std::source_location& = std::source_location::current());

void i_ThrowOnQueryResultError(duckdb::QueryResult&, const std::source_location& = std::source_location::current());

}
