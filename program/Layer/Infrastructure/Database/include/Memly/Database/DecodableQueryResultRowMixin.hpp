#pragma once

#include <concepts>
#include <tuple>

#include "Memly/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"

namespace Layer::Infrastructure::Database {

template <typename... ColumnType>
class DecodableQueryResultRowMixin : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
protected:
    explicit DecodableQueryResultRowMixin() noexcept
        : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{} {
    }

public:
    using DecodableQueryResultRowMixinType = DecodableQueryResultRowMixin;
    using QueryResultColumnTypeTuple = std::tuple<ColumnType...>;
};

template <typename QueryResultRowType>
concept DecodableQueryResultRow = requires {
    typename QueryResultRowType::DecodableQueryResultRowMixinType;
    typename QueryResultRowType::QueryResultColumnTypeTuple;
} and std::derived_from<QueryResultRowType, typename QueryResultRowType::DecodableQueryResultRowMixinType>;

}
