#pragma once

#include <concepts>
#include <tuple>

namespace Layer::Infrastructure::Database {

template <typename... ColumnType>
class DecodableQueryResultRowMixin {
protected:
    explicit constexpr DecodableQueryResultRowMixin() noexcept = default;

public:
    explicit DecodableQueryResultRowMixin(const DecodableQueryResultRowMixin&) = delete;
    auto operator=(const DecodableQueryResultRowMixin&) -> DecodableQueryResultRowMixin& = delete;

    explicit constexpr DecodableQueryResultRowMixin(DecodableQueryResultRowMixin&&) noexcept = default;
    auto operator=(DecodableQueryResultRowMixin&&) -> DecodableQueryResultRowMixin& = delete;

    ~DecodableQueryResultRowMixin() noexcept = default;

    using DecodableQueryResultRowMixinType = DecodableQueryResultRowMixin;
    using QueryResultColumnTypeTuple = std::tuple<ColumnType...>;
};

template <typename QueryResultRowType>
concept DecodableQueryResultRow = requires {
    typename QueryResultRowType::DecodableQueryResultRowMixinType;
    typename QueryResultRowType::QueryResultColumnTypeTuple;
} and std::derived_from<QueryResultRowType, typename QueryResultRowType::DecodableQueryResultRowMixinType>;

}
