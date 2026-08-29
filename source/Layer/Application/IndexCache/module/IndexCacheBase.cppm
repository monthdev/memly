module;

#include <memory>
#include <utility>

export module Memly.IndexCache.IndexCacheBase;

export namespace Memly::IndexCache {

template <typename IndexCacheDefinitionType>
class IndexCacheBase {
private:
    std::weak_ptr<typename IndexCacheDefinitionType::IndexType>
        m_IndexWeakPointer;

protected:
    explicit IndexCacheBase() noexcept : m_IndexWeakPointer{} {
    }

public:
    explicit IndexCacheBase(const IndexCacheBase&) = delete;
    IndexCacheBase&
    operator=(const IndexCacheBase&) = delete;

    explicit IndexCacheBase(IndexCacheBase&&) = delete;
    IndexCacheBase&
    operator=(IndexCacheBase&&) = delete;

    ~IndexCacheBase() noexcept = default;

    class [[nodiscard]] IndexCacheLease final {
        friend class IndexCacheBase;

    private:
        std::shared_ptr<typename IndexCacheDefinitionType::IndexType>
            m_IndexSharedPointer;

        explicit IndexCacheLease(
            std::shared_ptr<typename IndexCacheDefinitionType::IndexType>&&
                IndexSharedPointer
        ) noexcept : m_IndexSharedPointer{ std::move(IndexSharedPointer) } {
        }

    public:
        explicit IndexCacheLease(const IndexCacheLease&) = delete;
        IndexCacheLease&
        operator=(const IndexCacheLease&) = delete;

        explicit IndexCacheLease(IndexCacheLease&&) noexcept = default;
        IndexCacheLease&
        operator=(IndexCacheLease&&) = delete;

        ~IndexCacheLease() noexcept = default;
    };

    [[nodiscard]] IndexCacheLease
    AcquireLease() {
        std::shared_ptr<typename IndexCacheDefinitionType::IndexType>
            IndexSharedPointer{
                this->m_IndexWeakPointer.lock(),
            };
        if (not IndexSharedPointer) {
            IndexSharedPointer = std::make_shared<
                typename IndexCacheDefinitionType::IndexType>();
            this->m_IndexWeakPointer = IndexSharedPointer;
        }
        return IndexCacheLease{ std::move(IndexSharedPointer) };
    }

    void
    Refresh(
        const IndexCacheLease& IndexCacheLease,
        typename IndexCacheDefinitionType::IndexRefreshDataType&&
            IndexRefreshData
    ) {
        IndexCacheDefinitionType::RefreshIndex(
            *IndexCacheLease.m_IndexSharedPointer,
            std::move(IndexRefreshData)
        );
    }

    [[nodiscard]] const typename IndexCacheDefinitionType::IndexType&
    GetIndex(
        [[clang::lifetimebound]] const IndexCacheLease& IndexCacheLease
    ) const& noexcept {
        return *IndexCacheLease.m_IndexSharedPointer;
    }

    [[nodiscard]] const typename IndexCacheDefinitionType::IndexType&
    GetIndex(const IndexCacheLease&&) const& noexcept = delete;
};

}
