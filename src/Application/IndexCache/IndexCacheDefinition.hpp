#pragma once

#include <functional>
#include <utility>

#include "Support/SpecialMemberPolicy/NonInstantiableMixin.hpp"

namespace Application::IndexCache {

template <typename IndexObjectType, typename IndexRefreshDataObjectType, void (IndexObjectType::*RefreshIndexMethod)(IndexRefreshDataObjectType&&)>
class IndexCacheDefinition : private Support::SpecialMemberPolicy::NonInstantiableMixin {
public:
    explicit IndexCacheDefinition() = delete;

    using IndexType = IndexObjectType;
    using IndexRefreshDataType = IndexRefreshDataObjectType;

    static void RefreshIndex(IndexObjectType& Index, IndexRefreshDataObjectType&& IndexRefreshData) {
        std::invoke(RefreshIndexMethod, Index, std::move(IndexRefreshData));
    }
};

}
