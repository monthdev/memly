#pragma once

#include "Application/IndexCache/Deck/DeckForestSnapshotIndexCacheDefinition.hpp"
#include "Application/IndexCache/IndexCacheBase.hpp"

namespace Application::IndexCache::Deck {

class DeckForestSnapshotIndexCache final : public Application::IndexCache::IndexCacheBase<DeckForestSnapshotIndexCacheDefinition> {
public:
    DeckForestSnapshotIndexCache() noexcept
        : Application::IndexCache::IndexCacheBase<DeckForestSnapshotIndexCacheDefinition>{} {
    }

    DeckForestSnapshotIndexCache(const DeckForestSnapshotIndexCache&) = delete;
    DeckForestSnapshotIndexCache(DeckForestSnapshotIndexCache&&) = delete;
    DeckForestSnapshotIndexCache& operator=(const DeckForestSnapshotIndexCache&) = delete;
    DeckForestSnapshotIndexCache& operator=(DeckForestSnapshotIndexCache&&) = delete;
};

}
