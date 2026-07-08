#pragma once

#include "Application/IndexCache/Deck/DeckTreeSnapshotIndexCacheDefinition.hpp"
#include "Application/IndexCache/IndexCacheLifecycle.hpp"

namespace Application::IndexCache::Deck {

class DeckTreeSnapshotIndexCache final : public Application::IndexCache::IndexCacheLifecycle<DeckTreeSnapshotIndexCacheDefinition> {
public:
    DeckTreeSnapshotIndexCache() noexcept
        : Application::IndexCache::IndexCacheLifecycle<DeckTreeSnapshotIndexCacheDefinition>{} {
    }

    DeckTreeSnapshotIndexCache(const DeckTreeSnapshotIndexCache&) = delete;
    DeckTreeSnapshotIndexCache(DeckTreeSnapshotIndexCache&&) = delete;
    DeckTreeSnapshotIndexCache& operator=(const DeckTreeSnapshotIndexCache&) = delete;
    DeckTreeSnapshotIndexCache& operator=(DeckTreeSnapshotIndexCache&&) = delete;
};

}
