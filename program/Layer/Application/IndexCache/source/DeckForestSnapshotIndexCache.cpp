#include "Memly/IndexCache/DeckForestSnapshotIndexCache.hpp"

#include "Memly/IndexCache/DeckForestSnapshotIndexCacheDefinition.hpp"
#include "Memly/IndexCache/IndexCacheBase.hpp"

namespace Memly::IndexCache {

DeckForestSnapshotIndexCache::DeckForestSnapshotIndexCache() noexcept
    : IndexCacheBase<DeckForestSnapshotIndexCacheDefinition>{} {
}

}
