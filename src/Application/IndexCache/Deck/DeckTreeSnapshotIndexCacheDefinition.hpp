#pragma once

#include <vector>

#include "Application/Domain/Deck/Index/DeckTreeSnapshotIndex.hpp"
#include "Application/IndexCache/IndexCacheDefinition.hpp"

namespace Application::IndexCache::Deck {

class DeckTreeSnapshotIndexCacheDefinition final
    : public Application::IndexCache::IndexCacheDefinition<Application::Domain::Deck::Index::DeckTreeSnapshotIndex,
                                                           std::vector<Application::Domain::Deck::Index::DeckTreeSnapshotIndex::DeckTreeSnapshotNode>,
                                                           &Application::Domain::Deck::Index::DeckTreeSnapshotIndex::RefreshFromDeckTreeSnapshotNodes> {
public:
    DeckTreeSnapshotIndexCacheDefinition() = delete;
    DeckTreeSnapshotIndexCacheDefinition(const DeckTreeSnapshotIndexCacheDefinition&) = delete;
    DeckTreeSnapshotIndexCacheDefinition(DeckTreeSnapshotIndexCacheDefinition&&) = delete;
    DeckTreeSnapshotIndexCacheDefinition& operator=(const DeckTreeSnapshotIndexCacheDefinition&) = delete;
    DeckTreeSnapshotIndexCacheDefinition& operator=(DeckTreeSnapshotIndexCacheDefinition&&) = delete;
};

}
