#pragma once

#include <memory>
#include <utility>

namespace Application::IndexCache {

template <typename IndexCacheDefinitionType>
class IndexCacheBase;

template <typename IndexCacheDefinitionType>
class IndexCacheLease final {
public:
    IndexCacheLease() = delete;
    IndexCacheLease(const IndexCacheLease&) = delete;
    IndexCacheLease(IndexCacheLease&&) noexcept = default;
    IndexCacheLease& operator=(const IndexCacheLease&) = delete;
    IndexCacheLease& operator=(IndexCacheLease&&) = delete;

private:
    friend class IndexCacheBase<IndexCacheDefinitionType>;

    explicit IndexCacheLease(std::shared_ptr<typename IndexCacheDefinitionType::IndexType>&& IndexSharedPointer) noexcept
        : m_IndexSharedPointer{ std::move(IndexSharedPointer) } {
    }

    std::shared_ptr<typename IndexCacheDefinitionType::IndexType> m_IndexSharedPointer;
};

}
