#include "Layer/Application/IndexCache/Deck/DeckForestSnapshotIndexCache.hpp"

#include "Layer/Application/IndexCache/Deck/DeckForestSnapshotIndexCacheDefinition.hpp"
#include "Layer/Application/IndexCache/IndexCacheBase.hpp"

namespace Layer::Application::IndexCache::Deck {

DeckForestSnapshotIndexCache::DeckForestSnapshotIndexCache() noexcept
    : IndexCacheBase<DeckForestSnapshotIndexCacheDefinition>{} {
}

}
