#include "Layer/Infrastructure/Database/QueryResultDecoder.hpp"

#include <duckdb.hpp>

#include <initializer_list>
#include <string_view>
#include <utility>

#include "Support/Runtime/Exception/ThrowMemlyException.hpp"

namespace Layer::Infrastructure::Database {

[[nodiscard]] auto QueryResultDecoder::FetchNextDataChunk() -> duckdb::unique_ptr<duckdb::DataChunk> {
    duckdb::unique_ptr<duckdb::DataChunk> DataChunk{};
    duckdb::ErrorData ErrorData{};
    if (not this->m_QueryResult->TryFetch(DataChunk, ErrorData)) {
        Support::Runtime::Exception::ThrowMemlyException(std::initializer_list<std::string_view>{ ErrorData.Message() }, this->m_SourceLocation);
    }
    return duckdb::unique_ptr<duckdb::DataChunk>{ std::move(DataChunk) };
}

}
