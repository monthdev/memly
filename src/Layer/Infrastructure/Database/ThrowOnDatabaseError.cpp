#include "Layer/Infrastructure/Database/ThrowOnDatabaseError.hpp"

#include <duckdb.hpp>

#include <initializer_list>
#include <source_location>
#include <string_view>

#include "Support/Runtime/Exception/ThrowMemlyException.hpp"

namespace Layer::Infrastructure::Database {

void ThrowOnQueryResultError(duckdb::QueryResult& QueryResult, const std::source_location& SourceLocation) {
    if (QueryResult.HasError()) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ QueryResult.GetError() }, SourceLocation);
    }
}

void ThrowOnPreparedStatementError(duckdb::PreparedStatement& PreparedStatement, const std::source_location& SourceLocation) {
    if (PreparedStatement.HasError()) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ PreparedStatement.GetError() }, SourceLocation);
    }
}

}
