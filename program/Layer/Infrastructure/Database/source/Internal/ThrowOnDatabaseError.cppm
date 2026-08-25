module;

#include <duckdb.hpp>

#include <source_location>

export module Memly.Database.Internal.ThrowOnDatabaseError;

export namespace Memly::Database::Internal {

void ThrowOnPreparedStatementError(duckdb::PreparedStatement&, std::source_location = std::source_location::current());

void ThrowOnQueryResultError(duckdb::QueryResult&, std::source_location = std::source_location::current());

}
