#pragma once

#include <duckdb.hpp>

#include <cassert>
#include <cstddef>
#include <memory>
#include <optional>
#include <source_location>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "Memly/Database/DecodableQueryResultRowMixin.hpp"
#include "Memly/Database/DecodedQueryResult.hpp"

namespace Memly::Database {

class [[nodiscard]] QueryResultDecoder final {
private:
    std::unique_ptr<duckdb::QueryResult> m_QueryResult;
    std::source_location m_SourceLocation;

public:
    explicit QueryResultDecoder(std::unique_ptr<duckdb::QueryResult>&&, std::source_location) noexcept;

    explicit QueryResultDecoder(const QueryResultDecoder&) = delete;
    auto operator=(const QueryResultDecoder&) -> QueryResultDecoder& = delete;

    explicit QueryResultDecoder(QueryResultDecoder&&) = delete;
    auto operator=(QueryResultDecoder&&) -> QueryResultDecoder& = delete;

    ~QueryResultDecoder() noexcept = default;

    template <DecodableQueryResultRow QueryResultRowType>
    [[nodiscard]] auto DecodedTo() && -> DecodedQueryResult<QueryResultRowType> {
        assert(this->m_QueryResult->ColumnCount() == std::tuple_size_v<typename QueryResultRowType::QueryResultColumnTypeTuple>);
        std::vector<QueryResultRowType> QueryResultRowVector{};
        while (const duckdb::unique_ptr<duckdb::DataChunk> DataChunk{ this->FetchNextDataChunk() }) {
            const duckdb::unsafe_unique_array<duckdb::UnifiedVectorFormat> UnifiedVectorFormatArray{ DataChunk->ToUnifiedFormat() };
            QueryResultDecoder::VerifyDataChunkColumnPhysicalTypes<QueryResultRowType>(
                UnifiedVectorFormatArray, std::make_index_sequence<std::tuple_size_v<typename QueryResultRowType::QueryResultColumnTypeTuple>>{});
            for (duckdb::idx_t RowIndex{ 0 }; RowIndex < DataChunk->size(); ++RowIndex) {
                QueryResultRowVector.emplace_back(QueryResultDecoder::DecodeQueryResultRow<QueryResultRowType>(
                    UnifiedVectorFormatArray, RowIndex, std::make_index_sequence<std::tuple_size_v<typename QueryResultRowType::QueryResultColumnTypeTuple>>{}));
            }
        }
        return DecodedQueryResult<QueryResultRowType>{ std::move(QueryResultRowVector) };
    }

private:
    [[nodiscard]] auto FetchNextDataChunk() -> duckdb::unique_ptr<duckdb::DataChunk>;

    template <DecodableQueryResultRow DecodableQueryResultRowType, std::size_t... ColumnIndex>
    static void VerifyDataChunkColumnPhysicalTypes(const duckdb::unsafe_unique_array<duckdb::UnifiedVectorFormat>& UnifiedVectorFormatArray,
                                                   const std::index_sequence<ColumnIndex...>) {
        (QueryResultDecoder::VerifyUnifiedVectorFormatStorageType(
             // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
             UnifiedVectorFormatArray[ColumnIndex],
             std::type_identity<std::tuple_element_t<ColumnIndex, typename DecodableQueryResultRowType::QueryResultColumnTypeTuple>>{}),
         ...);
    }

    static void VerifyUnifiedVectorFormatStorageType(const duckdb::UnifiedVectorFormat&, std::type_identity<std::string>);

    template <typename ColumnType>
    static void VerifyUnifiedVectorFormatStorageType(const duckdb::UnifiedVectorFormat& UnifiedVectorFormat, const std::type_identity<ColumnType>) {
        UnifiedVectorFormat.VerifyVectorType<ColumnType>();
    }

    template <typename ColumnValueType>
    static void VerifyUnifiedVectorFormatStorageType(const duckdb::UnifiedVectorFormat& UnifiedVectorFormat,
                                                     const std::type_identity<std::optional<ColumnValueType>>) {
        QueryResultDecoder::VerifyUnifiedVectorFormatStorageType(UnifiedVectorFormat, std::type_identity<ColumnValueType>{});
    }

    template <DecodableQueryResultRow DecodableQueryResultRowType, std::size_t... ColumnIndex>
    [[nodiscard]] static auto DecodeQueryResultRow(const duckdb::unsafe_unique_array<duckdb::UnifiedVectorFormat>& UnifiedVectorFormatArray,
                                                   const duckdb::idx_t RowIndex,
                                                   const std::index_sequence<ColumnIndex...>) -> DecodableQueryResultRowType {
        return DecodableQueryResultRowType{ QueryResultDecoder::DecodeColumn(
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
            UnifiedVectorFormatArray[ColumnIndex],
            RowIndex,
            std::type_identity<std::tuple_element_t<ColumnIndex, typename DecodableQueryResultRowType::QueryResultColumnTypeTuple>>{})... };
    }

    template <typename ColumnType>
    [[nodiscard]] static auto
    DecodeColumn(const duckdb::UnifiedVectorFormat& UnifiedVectorFormat, const duckdb::idx_t RowIndex, const std::type_identity<ColumnType>) -> ColumnType {
        assert(UnifiedVectorFormat.validity.RowIsValid(RowIndex));
        return ColumnType{ QueryResultDecoder::DecodeColumnValue(UnifiedVectorFormat, RowIndex, std::type_identity<ColumnType>{}) };
    }

    template <typename ColumnValueType>
    [[nodiscard]] static auto DecodeColumn(const duckdb::UnifiedVectorFormat& UnifiedVectorFormat,
                                           const duckdb::idx_t RowIndex,
                                           const std::type_identity<std::optional<ColumnValueType>>) -> std::optional<ColumnValueType> {
        if (not UnifiedVectorFormat.validity.RowIsValid(RowIndex)) {
            return std::optional<ColumnValueType>{};
        }
        return std::optional<ColumnValueType>{ QueryResultDecoder::DecodeColumnValue(UnifiedVectorFormat, RowIndex, std::type_identity<ColumnValueType>{}) };
    }

#pragma clang unsafe_buffer_usage begin

    template <typename ColumnValueType>
    [[nodiscard]] static auto DecodeColumnValue(const duckdb::UnifiedVectorFormat& UnifiedVectorFormat,
                                                const duckdb::idx_t RowIndex,
                                                const std::type_identity<ColumnValueType>) -> ColumnValueType {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return ColumnValueType{ duckdb::UnifiedVectorFormat::GetDataUnsafe<ColumnValueType>(UnifiedVectorFormat)[RowIndex] };
    }

#pragma clang unsafe_buffer_usage end

    [[nodiscard]] static auto DecodeColumnValue(const duckdb::UnifiedVectorFormat&, duckdb::idx_t, std::type_identity<std::string>) -> std::string;
};

}
