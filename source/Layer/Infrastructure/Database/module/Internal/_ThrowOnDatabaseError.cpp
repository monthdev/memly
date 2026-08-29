module;

#include <duckdb.hpp>

#include <initializer_list>
#include <source_location>
#include <string_view>

module Memly.Database.Internal.ThrowOnDatabaseError;

import Memly.Exception.MemlyException;

namespace Memly::Database::Internal {

void
ThrowOnPreparedStatementError(
    duckdb::PreparedStatement& PreparedStatement,
    const std::source_location SourceLocation
) {
    if (PreparedStatement.HasError()) {
        throw Exception::MemlyException{
            std::initializer_list<std::string_view>{
                PreparedStatement.GetError(),
            },
            SourceLocation
        };
    }
}

void
ThrowOnQueryResultError(
    duckdb::QueryResult& QueryResult,
    const std::source_location SourceLocation
) {
    if (QueryResult.HasError()) {
        throw Exception::MemlyException{
            std::initializer_list<std::string_view>{ QueryResult.GetError() },
            SourceLocation
        };
    }
}

}
