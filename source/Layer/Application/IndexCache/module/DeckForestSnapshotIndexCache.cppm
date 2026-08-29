export module Memly.IndexCache.DeckForestSnapshotIndexCache;

import Memly.IndexCache.DeckForestSnapshotIndexCacheDefinition;
import Memly.IndexCache.IndexCacheBase;

export namespace Memly::IndexCache {

class DeckForestSnapshotIndexCache final
    : public IndexCacheBase<DeckForestSnapshotIndexCacheDefinition> {
public:
    explicit DeckForestSnapshotIndexCache() noexcept = default;

    explicit DeckForestSnapshotIndexCache(
        const DeckForestSnapshotIndexCache&
    ) = delete;
    DeckForestSnapshotIndexCache&
    operator=(const DeckForestSnapshotIndexCache&) = delete;

    explicit DeckForestSnapshotIndexCache(
        DeckForestSnapshotIndexCache&&
    ) = delete;
    DeckForestSnapshotIndexCache&
    operator=(DeckForestSnapshotIndexCache&&) = delete;

    ~DeckForestSnapshotIndexCache() noexcept = default;
};

}
