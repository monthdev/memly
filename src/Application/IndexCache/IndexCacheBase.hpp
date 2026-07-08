#pragma once

#include <memory>
#include <utility>

namespace Application::IndexCache {

template <typename IndexCacheDefinitionType>
class IndexCacheBase {
public:
    class IndexCacheLease final {
    public:
        IndexCacheLease() = delete;
        IndexCacheLease(const IndexCacheLease&) = delete;
        IndexCacheLease(IndexCacheLease&&) noexcept = default;
        IndexCacheLease& operator=(const IndexCacheLease&) = delete;
        IndexCacheLease& operator=(IndexCacheLease&&) = delete;

    private:
        friend class IndexCacheBase;

        explicit IndexCacheLease(std::shared_ptr<typename IndexCacheDefinitionType::IndexType>&& IndexSharedPointer) noexcept
            : m_IndexSharedPointer{ std::move(IndexSharedPointer) } {
        }

        std::shared_ptr<typename IndexCacheDefinitionType::IndexType> m_IndexSharedPointer;
    };

    [[nodiscard]] IndexCacheLease AcquireLease() {
        std::shared_ptr<typename IndexCacheDefinitionType::IndexType> IndexSharedPointer{ m_IndexWeakPointer.lock() };
        if (IndexSharedPointer == nullptr) {
            IndexSharedPointer = std::make_shared<typename IndexCacheDefinitionType::IndexType>();
            m_IndexWeakPointer = IndexSharedPointer;
        }
        return IndexCacheLease{ std::move(IndexSharedPointer) };
    }

    void Refresh(const IndexCacheLease& IndexCacheLease, typename IndexCacheDefinitionType::IndexRefreshDataType&& IndexRefreshData) {
        IndexCacheDefinitionType::RefreshIndex(*IndexCacheLease.m_IndexSharedPointer, std::move(IndexRefreshData));
    }

    [[nodiscard]] const typename IndexCacheDefinitionType::IndexType& GetIndex(const IndexCacheLease& IndexCacheLease) const noexcept {
        return *IndexCacheLease.m_IndexSharedPointer;
    }

protected:
    IndexCacheBase() noexcept
        : m_IndexWeakPointer{} {
    }

    IndexCacheBase(const IndexCacheBase&) = delete;
    IndexCacheBase(IndexCacheBase&&) = delete;
    IndexCacheBase& operator=(const IndexCacheBase&) = delete;
    IndexCacheBase& operator=(IndexCacheBase&&) = delete;

private:
    std::weak_ptr<typename IndexCacheDefinitionType::IndexType> m_IndexWeakPointer;
};

}
