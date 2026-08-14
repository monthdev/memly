#pragma once

#include <duckdb.hpp>

#include <source_location>

namespace Memly::Database {

void ThrowOnPreparedStatementError(duckdb::PreparedStatement&, const std::source_location& = std::source_location::current());

void ThrowOnQueryResultError(duckdb::QueryResult&, const std::source_location& = std::source_location::current());

}
