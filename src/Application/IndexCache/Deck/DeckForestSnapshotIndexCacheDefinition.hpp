#pragma once

#include <vector>

#include "Application/Domain/Deck/Index/DeckForestSnapshotIndex.hpp"
#include "Application/IndexCache/IndexCacheDefinition.hpp"

namespace Application::IndexCache::Deck {

class DeckForestSnapshotIndexCacheDefinition final
    : public Application::IndexCache::IndexCacheDefinition<Application::Domain::Deck::Index::DeckForestSnapshotIndex,
                                                           std::vector<Application::Domain::Deck::Index::DeckForestSnapshotIndex::DeckForestSnapshotNode>,
                                                           &Application::Domain::Deck::Index::DeckForestSnapshotIndex::RefreshFromDeckForestSnapshotNodes> {
public:
    DeckForestSnapshotIndexCacheDefinition() = delete;
    DeckForestSnapshotIndexCacheDefinition(const DeckForestSnapshotIndexCacheDefinition&) = delete;
    DeckForestSnapshotIndexCacheDefinition(DeckForestSnapshotIndexCacheDefinition&&) = delete;
    DeckForestSnapshotIndexCacheDefinition& operator=(const DeckForestSnapshotIndexCacheDefinition&) = delete;
    DeckForestSnapshotIndexCacheDefinition& operator=(DeckForestSnapshotIndexCacheDefinition&&) = delete;
};

}
