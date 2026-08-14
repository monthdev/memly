#pragma once

#include <functional>
#include <utility>

namespace Memly::IndexCache {

template <typename IndexObjectType, typename IndexRefreshDataObjectType, void (IndexObjectType::*RefreshIndexMethod)(IndexRefreshDataObjectType&&)>
class IndexCacheDefinition {
public:
    explicit IndexCacheDefinition() = delete;

    explicit IndexCacheDefinition(const IndexCacheDefinition&) = delete;
    auto operator=(const IndexCacheDefinition&) -> IndexCacheDefinition& = delete;

    explicit IndexCacheDefinition(IndexCacheDefinition&&) = delete;
    auto operator=(IndexCacheDefinition&&) -> IndexCacheDefinition& = delete;

    ~IndexCacheDefinition() noexcept = default;

    using IndexType = IndexObjectType;
    using IndexRefreshDataType = IndexRefreshDataObjectType;

    static void RefreshIndex(IndexObjectType& Index, IndexRefreshDataObjectType&& IndexRefreshData) {
        std::invoke(RefreshIndexMethod, Index, std::move(IndexRefreshData));
    }
};

}
