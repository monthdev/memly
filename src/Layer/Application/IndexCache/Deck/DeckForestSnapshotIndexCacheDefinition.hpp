#pragma once

#include <vector>

#include "Layer/Application/Domain/Deck/Index/DeckForestSnapshotIndex.hpp"
#include "Layer/Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"
#include "Layer/Application/IndexCache/IndexCacheDefinition.hpp"

namespace Layer::Application::IndexCache::Deck {

class DeckForestSnapshotIndexCacheDefinition final
    : public IndexCacheDefinition<Domain::Deck::Index::DeckForestSnapshotIndex,
                                  std::vector<Domain::Deck::Index::DeckForestSnapshotNode>,
                                  &Domain::Deck::Index::DeckForestSnapshotIndex::RefreshFromDeckForestSnapshotNodes> {
public:
    explicit DeckForestSnapshotIndexCacheDefinition() = delete;
};

}
