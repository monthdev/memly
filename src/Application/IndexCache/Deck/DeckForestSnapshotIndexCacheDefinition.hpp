#pragma once

#include <vector>

#include "Application/Domain/Deck/Index/DeckForestSnapshotIndex.hpp"
#include "Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"
#include "Application/IndexCache/IndexCacheDefinition.hpp"

namespace Application::IndexCache::Deck {

class DeckForestSnapshotIndexCacheDefinition final
    : public Application::IndexCache::IndexCacheDefinition<Application::Domain::Deck::Index::DeckForestSnapshotIndex,
                                                           std::vector<Application::Domain::Deck::Index::DeckForestSnapshotNode>,
                                                           &Application::Domain::Deck::Index::DeckForestSnapshotIndex::RefreshFromDeckForestSnapshotNodes> {
public:
    explicit DeckForestSnapshotIndexCacheDefinition() = delete;
};

}
