#pragma once

#include "Layer/Application/IndexCache/Deck/DeckForestSnapshotIndexCacheDefinition.hpp"
#include "Layer/Application/IndexCache/IndexCacheBase.hpp"

namespace Layer::Application::IndexCache::Deck {

class DeckForestSnapshotIndexCache final : public IndexCacheBase<DeckForestSnapshotIndexCacheDefinition> {
public:
    explicit DeckForestSnapshotIndexCache() noexcept;
};

}
