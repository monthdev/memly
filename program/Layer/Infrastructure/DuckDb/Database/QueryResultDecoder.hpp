#pragma once

#include <duckdb.hpp>

#include <cassert>
#include <cstddef>
#include <memory>
#include <optional>
#include <source_location>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "Layer/Infrastructure/DuckDb/Database/DecodableQueryResultRowMixin.hpp"
#include "Layer/Infrastructure/DuckDb/Database/DecodedQueryResult.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

class DatabaseRuntime;
class PreparedStatementExecution;

class [[nodiscard]] QueryResultDecoder final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    friend class DatabaseRuntime;
    friend class PreparedStatementExecution;

private:
    std::unique_ptr<duckdb::QueryResult> m_QueryResult;
    const std::source_location& m_SourceLocation;

    explicit QueryResultDecoder(std::unique_ptr<duckdb::QueryResult>&& QueryResult, const std::source_location& SourceLocation) noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_QueryResult{ std::move(QueryResult) }
        , m_SourceLocation{ SourceLocation } {
    }

public:
    template <DecodableQueryResultRow QueryResultRowType>
    [[nodiscard]] auto DecodedTo() && -> DecodedQueryResult<QueryResultRowType> {
        assert(this->m_QueryResult->ColumnCount() == std::tuple_size_v<typename QueryResultRowType::QueryResultColumnTypeTuple>);
        std::vector<QueryResultRowType> QueryResultRowVector{};
        while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{ this->FetchNextDataChunk() }) {
            for (duckdb::idx_t RowIndex{ 0 }; RowIndex < DataChunk->size(); ++RowIndex) {
                QueryResultRowVector.emplace_back(DecodeQueryResultRow<QueryResultRowType>(
                    *DataChunk, RowIndex, std::make_index_sequence<std::tuple_size_v<typename QueryResultRowType::QueryResultColumnTypeTuple>>{}));
            }
        }
        return DecodedQueryResult<QueryResultRowType>{ std::move(QueryResultRowVector) };
    }

private:
    [[nodiscard]] auto FetchNextDataChunk() -> duckdb::unique_ptr<duckdb::DataChunk>;

    template <DecodableQueryResultRow DecodableQueryResultRowType, std::size_t... ColumnIndex>
    [[nodiscard]] static auto DecodeQueryResultRow(duckdb::DataChunk& DataChunk, const duckdb::idx_t RowIndex, const std::index_sequence<ColumnIndex...>)
        -> DecodableQueryResultRowType {
        return DecodableQueryResultRowType{ DecodeColumn(
            DataChunk.GetValue(ColumnIndex, RowIndex),
            std::type_identity<std::tuple_element_t<ColumnIndex, typename DecodableQueryResultRowType::QueryResultColumnTypeTuple>>{})... };
    }

    template <typename ColumnType>
    [[nodiscard]] static auto DecodeColumn(const duckdb::Value& DuckDbValue, const std::type_identity<ColumnType>) -> ColumnType {
        assert(not DuckDbValue.IsNull());
        return ColumnType{ DuckDbValue.GetValue<ColumnType>() };
    }

    template <typename ColumnValueType>
    [[nodiscard]] static auto DecodeColumn(const duckdb::Value& DuckDbValue, const std::type_identity<std::optional<ColumnValueType>>)
        -> std::optional<ColumnValueType> {
        if (DuckDbValue.IsNull()) {
            return std::optional<ColumnValueType>{};
        }
        return std::optional<ColumnValueType>{ DuckDbValue.GetValue<ColumnValueType>() };
    }
};

}
