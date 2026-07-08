#pragma once

#include <functional>
#include <utility>

namespace Application::IndexCache {

template <typename IndexObjectType, typename IndexRefreshDataObjectType, void (IndexObjectType::*RefreshIndexMethod)(IndexRefreshDataObjectType&&)>
class IndexCacheDefinition {
public:
    IndexCacheDefinition() = delete;
    IndexCacheDefinition(const IndexCacheDefinition&) = delete;
    IndexCacheDefinition(IndexCacheDefinition&&) = delete;
    IndexCacheDefinition& operator=(const IndexCacheDefinition&) = delete;
    IndexCacheDefinition& operator=(IndexCacheDefinition&&) = delete;

    using IndexType = IndexObjectType;
    using IndexRefreshDataType = IndexRefreshDataObjectType;

    static void RefreshIndex(IndexObjectType& Index, IndexRefreshDataObjectType&& IndexRefreshData) {
        std::invoke(RefreshIndexMethod, Index, std::move(IndexRefreshData));
    }
};

}
