#pragma once

#include <memory>
#include <utility>

namespace Application::IndexCache {

template <typename IndexCacheDefinitionType>
class IndexCacheBase {
private:
    std::weak_ptr<typename IndexCacheDefinitionType::IndexType> m_IndexWeakPointer;

protected:
    explicit IndexCacheBase() noexcept
        : m_IndexWeakPointer{} {
    }

public:
    explicit IndexCacheBase(const IndexCacheBase&) = delete;
    explicit IndexCacheBase(IndexCacheBase&&) = delete;
    auto operator=(const IndexCacheBase&) -> IndexCacheBase& = delete;
    auto operator=(IndexCacheBase&&) -> IndexCacheBase& = delete;

    class [[nodiscard]] IndexCacheLease final {
        friend class IndexCacheBase;

    private:
        std::shared_ptr<typename IndexCacheDefinitionType::IndexType> m_IndexSharedPointer;

        explicit IndexCacheLease(std::shared_ptr<typename IndexCacheDefinitionType::IndexType>&& IndexSharedPointer) noexcept
            : m_IndexSharedPointer{ std::move(IndexSharedPointer) } {
        }

    public:
        explicit IndexCacheLease() = delete;
        explicit IndexCacheLease(const IndexCacheLease&) = delete;
        explicit IndexCacheLease(IndexCacheLease&&) noexcept = default;
        auto operator=(const IndexCacheLease&) -> IndexCacheLease& = delete;
        auto operator=(IndexCacheLease&&) -> IndexCacheLease& = delete;
    };

    [[nodiscard]] auto AcquireLease() -> IndexCacheLease {
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

    [[nodiscard]] auto GetIndex(const IndexCacheLease& IndexCacheLease) const noexcept -> const typename IndexCacheDefinitionType::IndexType& {
        return *IndexCacheLease.m_IndexSharedPointer;
    }
};

}
