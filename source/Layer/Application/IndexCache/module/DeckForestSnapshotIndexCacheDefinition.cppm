module;

#include <vector>

export module Memly.IndexCache.DeckForestSnapshotIndexCacheDefinition;

import Memly.Domain.DeckForestSnapshotIndex;
import Memly.Domain.DeckForestSnapshotNode;
import Memly.IndexCache.IndexCacheDefinition;

export namespace Memly::IndexCache {

class DeckForestSnapshotIndexCacheDefinition final : public IndexCacheDefinition<Domain::DeckForestSnapshotIndex,
                                                                                 std::vector<Domain::DeckForestSnapshotNode>,
                                                                                 &Domain::DeckForestSnapshotIndex::RefreshFromDeckForestSnapshotNodes> {
public:
    explicit DeckForestSnapshotIndexCacheDefinition() = delete;

    explicit DeckForestSnapshotIndexCacheDefinition(const DeckForestSnapshotIndexCacheDefinition&) = delete;
    auto operator=(const DeckForestSnapshotIndexCacheDefinition&) -> DeckForestSnapshotIndexCacheDefinition& = delete;

    explicit DeckForestSnapshotIndexCacheDefinition(DeckForestSnapshotIndexCacheDefinition&&) = delete;
    auto operator=(DeckForestSnapshotIndexCacheDefinition&&) -> DeckForestSnapshotIndexCacheDefinition& = delete;

    ~DeckForestSnapshotIndexCacheDefinition() noexcept = default;
};

}
