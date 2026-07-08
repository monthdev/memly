#pragma once

#include <functional>
#include <utility>

namespace Application::IndexCache {

template <typename IndexCacheDefinitionType>
class IndexCacheLifecycle;

template <typename IndexCacheDefinitionType,
          typename IndexObjectType,
          typename CacheLeaseObjectType,
          typename IndexRefreshDataObjectType,
          void (IndexObjectType::*RefreshIndexMethod)(IndexRefreshDataObjectType&&)>
class IndexCacheDefinition {
public:
    IndexCacheDefinition() = delete;
    IndexCacheDefinition(const IndexCacheDefinition&) = delete;
    IndexCacheDefinition(IndexCacheDefinition&&) = delete;
    IndexCacheDefinition& operator=(const IndexCacheDefinition&) = delete;
    IndexCacheDefinition& operator=(IndexCacheDefinition&&) = delete;

    using IndexType = IndexObjectType;
    using CacheLeaseType = CacheLeaseObjectType;
    using IndexRefreshDataType = IndexRefreshDataObjectType;

private:
    friend class IndexCacheLifecycle<IndexCacheDefinitionType>;

    static void RefreshIndex(IndexObjectType& Index, IndexRefreshDataObjectType&& IndexRefreshData) {
        std::invoke(RefreshIndexMethod, Index, std::move(IndexRefreshData));
    }
};

}
