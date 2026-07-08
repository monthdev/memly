#pragma once

#include <memory>
#include <utility>

#include "Application/IndexCache/IndexCacheLease.hpp"

namespace Application::IndexCache {

template <typename IndexCacheDefinitionType>
class IndexCacheLifecycle {
public:
    [[nodiscard]] typename IndexCacheDefinitionType::CacheLeaseType AcquireLease() {
        std::shared_ptr<typename IndexCacheDefinitionType::IndexType> IndexSharedPointer{ m_IndexWeakPointer.lock() };
        if (IndexSharedPointer == nullptr) {
            IndexSharedPointer = std::make_shared<typename IndexCacheDefinitionType::IndexType>();
            m_IndexWeakPointer = IndexSharedPointer;
        }
        return typename IndexCacheDefinitionType::CacheLeaseType{ IndexCacheLease<IndexCacheDefinitionType>{ std::move(IndexSharedPointer) } };
    }

    void Refresh(const typename IndexCacheDefinitionType::CacheLeaseType& IndexCacheLease,
                 typename IndexCacheDefinitionType::IndexRefreshDataType&& IndexRefreshData) {
        IndexCacheDefinitionType::RefreshIndex(*IndexCacheLease.m_IndexCacheLease.m_IndexSharedPointer, std::move(IndexRefreshData));
    }

    [[nodiscard]] const typename IndexCacheDefinitionType::IndexType&
    GetIndex(const typename IndexCacheDefinitionType::CacheLeaseType& IndexCacheLease) const noexcept {
        return *IndexCacheLease.m_IndexCacheLease.m_IndexSharedPointer;
    }

protected:
    IndexCacheLifecycle() noexcept
        : m_IndexWeakPointer{} {
    }

    ~IndexCacheLifecycle() noexcept = default;
    IndexCacheLifecycle(const IndexCacheLifecycle&) = delete;
    IndexCacheLifecycle(IndexCacheLifecycle&&) = delete;
    IndexCacheLifecycle& operator=(const IndexCacheLifecycle&) = delete;
    IndexCacheLifecycle& operator=(IndexCacheLifecycle&&) = delete;

private:
    std::weak_ptr<typename IndexCacheDefinitionType::IndexType> m_IndexWeakPointer;
};

}
