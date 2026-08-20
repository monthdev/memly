module;

#include <duckdb.hpp>

#include <source_location>

export module Memly.Database:ThrowOnDatabaseError;

export namespace Memly::Database {

void i_ThrowOnPreparedStatementError(duckdb::PreparedStatement&, std::source_location = std::source_location::current());

void i_ThrowOnQueryResultError(duckdb::QueryResult&, std::source_location = std::source_location::current());

}
