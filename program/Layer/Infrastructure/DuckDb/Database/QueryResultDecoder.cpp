#include "Layer/Infrastructure/DuckDb/Database/QueryResultDecoder.hpp"

#include <duckdb.hpp>

#include <initializer_list>
#include <memory>
#include <source_location>
#include <string_view>
#include <utility>

#include "Support/Runtime/Exception/ThrowMemlyException.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

QueryResultDecoder::QueryResultDecoder(std::unique_ptr<duckdb::QueryResult>&& QueryResult, const std::source_location& SourceLocation) noexcept
    : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_QueryResult{ std::move(QueryResult) }
    , m_SourceLocation{ SourceLocation } {
}

[[nodiscard]] auto QueryResultDecoder::FetchNextDataChunk() -> duckdb::unique_ptr<duckdb::DataChunk> {
    duckdb::unique_ptr<duckdb::DataChunk> DataChunk{};
    duckdb::ErrorData ErrorData{};
    if (not this->m_QueryResult->TryFetch(DataChunk, ErrorData)) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ ErrorData.Message() }, this->m_SourceLocation);
    }
    return duckdb::unique_ptr<duckdb::DataChunk>{ std::move(DataChunk) };
}

}
