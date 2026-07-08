#pragma once

#include "Application/IndexCache/Deck/DeckTreeSnapshotIndexCacheDefinition.hpp"
#include "Application/IndexCache/IndexCacheBase.hpp"

namespace Application::IndexCache::Deck {

class DeckTreeSnapshotIndexCache final : public Application::IndexCache::IndexCacheBase<DeckTreeSnapshotIndexCacheDefinition> {
public:
    DeckTreeSnapshotIndexCache() noexcept
        : Application::IndexCache::IndexCacheBase<DeckTreeSnapshotIndexCacheDefinition>{} {
    }

    DeckTreeSnapshotIndexCache(const DeckTreeSnapshotIndexCache&) = delete;
    DeckTreeSnapshotIndexCache(DeckTreeSnapshotIndexCache&&) = delete;
    DeckTreeSnapshotIndexCache& operator=(const DeckTreeSnapshotIndexCache&) = delete;
    DeckTreeSnapshotIndexCache& operator=(DeckTreeSnapshotIndexCache&&) = delete;
};

}
