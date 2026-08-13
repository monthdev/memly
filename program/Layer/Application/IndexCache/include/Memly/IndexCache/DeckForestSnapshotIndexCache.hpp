#pragma once

#include "Memly/IndexCache/DeckForestSnapshotIndexCacheDefinition.hpp"
#include "Memly/IndexCache/IndexCacheBase.hpp"

namespace Layer::Application::IndexCache {

class DeckForestSnapshotIndexCache final : public IndexCacheBase<DeckForestSnapshotIndexCacheDefinition> {
public:
    explicit DeckForestSnapshotIndexCache() noexcept;

    explicit DeckForestSnapshotIndexCache(const DeckForestSnapshotIndexCache&) = delete;
    auto operator=(const DeckForestSnapshotIndexCache&) -> DeckForestSnapshotIndexCache& = delete;

    explicit DeckForestSnapshotIndexCache(DeckForestSnapshotIndexCache&&) = delete;
    auto operator=(DeckForestSnapshotIndexCache&&) -> DeckForestSnapshotIndexCache& = delete;

    ~DeckForestSnapshotIndexCache() noexcept = default;
};

}
