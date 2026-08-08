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

#include "Layer/Infrastructure/DuckDb/Database/DecodableQueryResultRowMixin.hpp"
#include "Layer/Infrastructure/DuckDb/Database/DecodedQueryResult.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Infrastructure::DuckDb::Database {

class PreparedStatementExecution;

class [[nodiscard]] QueryResultDecoder final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
    friend class PreparedStatementExecution;

private:
    std::unique_ptr<duckdb::QueryResult> m_QueryResult;
    const std::source_location& m_SourceLocation;

    explicit QueryResultDecoder(std::unique_ptr<duckdb::QueryResult>&&, const std::source_location&) noexcept;

public:
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
        const duckdb::idx_t SelectedRowIndex{ UnifiedVectorFormat.sel->get_index(RowIndex) };
        assert(UnifiedVectorFormat.validity.RowIsValid(SelectedRowIndex));
        return ColumnType{ QueryResultDecoder::DecodeColumnValue(UnifiedVectorFormat, SelectedRowIndex, std::type_identity<ColumnType>{}) };
    }

    template <typename ColumnValueType>
    [[nodiscard]] static auto DecodeColumn(const duckdb::UnifiedVectorFormat& UnifiedVectorFormat,
                                           const duckdb::idx_t RowIndex,
                                           const std::type_identity<std::optional<ColumnValueType>>) -> std::optional<ColumnValueType> {
        const duckdb::idx_t SelectedRowIndex{ UnifiedVectorFormat.sel->get_index(RowIndex) };
        if (not UnifiedVectorFormat.validity.RowIsValid(SelectedRowIndex)) {
            return std::optional<ColumnValueType>{};
        }
        return std::optional<ColumnValueType>{ QueryResultDecoder::DecodeColumnValue(
            UnifiedVectorFormat, SelectedRowIndex, std::type_identity<ColumnValueType>{}) };
    }

#pragma clang unsafe_buffer_usage begin

    template <typename ColumnValueType>
    [[nodiscard]] static auto DecodeColumnValue(const duckdb::UnifiedVectorFormat& UnifiedVectorFormat,
                                                const duckdb::idx_t SelectedRowIndex,
                                                const std::type_identity<ColumnValueType>) -> ColumnValueType {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return ColumnValueType{ duckdb::UnifiedVectorFormat::GetDataUnsafe<ColumnValueType>(UnifiedVectorFormat)[SelectedRowIndex] };
    }

#pragma clang unsafe_buffer_usage end

    [[nodiscard]] static auto DecodeColumnValue(const duckdb::UnifiedVectorFormat&, duckdb::idx_t, std::type_identity<std::string>) -> std::string;
};

}
