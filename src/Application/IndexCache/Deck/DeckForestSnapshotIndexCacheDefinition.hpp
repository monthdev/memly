#pragma once

#include <vector>

#include "Application/Domain/Deck/Index/DeckForestSnapshotIndex.hpp"
#include "Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"
#include "Application/IndexCache/IndexCacheDefinition.hpp"

namespace Application::IndexCache::Deck {

class DeckForestSnapshotIndexCacheDefinition final
    : public IndexCacheDefinition<Domain::Deck::Index::DeckForestSnapshotIndex,
                                  std::vector<Domain::Deck::Index::DeckForestSnapshotNode>,
                                  &Domain::Deck::Index::DeckForestSnapshotIndex::RefreshFromDeckForestSnapshotNodes> {
public:
    explicit DeckForestSnapshotIndexCacheDefinition() = delete;
};

}
