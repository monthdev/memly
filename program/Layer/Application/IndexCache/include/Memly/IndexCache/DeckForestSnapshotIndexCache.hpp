#pragma once

#include "Memly/IndexCache/DeckForestSnapshotIndexCacheDefinition.hpp"
#include "Memly/IndexCache/IndexCacheBase.hpp"

namespace Memly::IndexCache {

class DeckForestSnapshotIndexCache final : public IndexCacheBase<DeckForestSnapshotIndexCacheDefinition> {
public:
    explicit DeckForestSnapshotIndexCache() noexcept = default;

    explicit DeckForestSnapshotIndexCache(const DeckForestSnapshotIndexCache&) = delete;
    auto operator=(const DeckForestSnapshotIndexCache&) -> DeckForestSnapshotIndexCache& = delete;

    explicit DeckForestSnapshotIndexCache(DeckForestSnapshotIndexCache&&) = delete;
    auto operator=(DeckForestSnapshotIndexCache&&) -> DeckForestSnapshotIndexCache& = delete;

    ~DeckForestSnapshotIndexCache() noexcept = default;
};

}
