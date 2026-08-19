#include "_ThrowOnDatabaseError.hpp"

#include <duckdb.hpp>

#include <initializer_list>
#include <source_location>
#include <string_view>

#include "Memly/Exception/MemlyException.hpp"

namespace Memly::Database {

void i_ThrowOnPreparedStatementError(duckdb::PreparedStatement& PreparedStatement, const std::source_location SourceLocation) {
    if (PreparedStatement.HasError()) {
        throw Exception::MemlyException{ std::initializer_list<std::string_view>{ PreparedStatement.GetError() }, SourceLocation };
    }
}

void i_ThrowOnQueryResultError(duckdb::QueryResult& QueryResult, const std::source_location SourceLocation) {
    if (QueryResult.HasError()) {
        throw Exception::MemlyException{ std::initializer_list<std::string_view>{ QueryResult.GetError() }, SourceLocation };
    }
}

}
