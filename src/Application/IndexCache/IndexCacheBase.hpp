#pragma once

#include <memory>
#include <utility>

#include "Support/SpecialMemberPolicy/NoCopyMoveConstructOnlyMixin.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Application::IndexCache {

template <typename IndexCacheDefinitionType>
class IndexCacheBase : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    std::weak_ptr<typename IndexCacheDefinitionType::IndexType> m_IndexWeakPointer;

protected:
    explicit IndexCacheBase() noexcept
        : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
        , m_IndexWeakPointer{} {
    }

public:
    class [[nodiscard]] IndexCacheLease final : private Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin {
        friend class IndexCacheBase;

    private:
        std::shared_ptr<typename IndexCacheDefinitionType::IndexType> m_IndexSharedPointer;

        explicit IndexCacheLease(std::shared_ptr<typename IndexCacheDefinitionType::IndexType>&& IndexSharedPointer) noexcept
            : Support::SpecialMemberPolicy::NoCopyMoveConstructOnlyMixin{}
            , m_IndexSharedPointer{ std::move(IndexSharedPointer) } {
        }
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
