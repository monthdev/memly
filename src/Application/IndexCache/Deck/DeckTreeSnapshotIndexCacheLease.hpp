#pragma once

#include <utility>

#include "Application/IndexCache/Deck/DeckTreeSnapshotIndexCacheDefinition.hpp"
#include "Application/IndexCache/IndexCacheLease.hpp"

namespace Application::IndexCache {
template <typename IndexCacheDefinitionType>
class IndexCacheLifecycle;
}

namespace Application::IndexCache::Deck {

class DeckTreeSnapshotIndexCacheLease final {
public:
    DeckTreeSnapshotIndexCacheLease() = delete;
    DeckTreeSnapshotIndexCacheLease(const DeckTreeSnapshotIndexCacheLease&) = delete;
    DeckTreeSnapshotIndexCacheLease(DeckTreeSnapshotIndexCacheLease&&) noexcept = default;
    DeckTreeSnapshotIndexCacheLease& operator=(const DeckTreeSnapshotIndexCacheLease&) = delete;
    DeckTreeSnapshotIndexCacheLease& operator=(DeckTreeSnapshotIndexCacheLease&&) = delete;

private:
    friend class Application::IndexCache::IndexCacheLifecycle<DeckTreeSnapshotIndexCacheDefinition>;

    explicit DeckTreeSnapshotIndexCacheLease(Application::IndexCache::IndexCacheLease<DeckTreeSnapshotIndexCacheDefinition>&& IndexCacheLease) noexcept
        : m_IndexCacheLease{ std::move(IndexCacheLease) } {
    }

    Application::IndexCache::IndexCacheLease<DeckTreeSnapshotIndexCacheDefinition> m_IndexCacheLease;
};

}
