#pragma once

#include "Application/IndexCache/Deck/DeckForestSnapshotIndexCacheDefinition.hpp"
#include "Application/IndexCache/IndexCacheBase.hpp"

namespace Application::IndexCache::Deck {

class DeckForestSnapshotIndexCache final : public IndexCacheBase<DeckForestSnapshotIndexCacheDefinition> {
public:
    explicit DeckForestSnapshotIndexCache() noexcept
        : IndexCacheBase<DeckForestSnapshotIndexCacheDefinition>{} {
    }
};

}
