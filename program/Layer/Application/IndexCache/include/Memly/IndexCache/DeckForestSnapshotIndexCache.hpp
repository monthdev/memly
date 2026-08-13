#pragma once

#include "Memly/IndexCache/DeckForestSnapshotIndexCacheDefinition.hpp"
#include "Memly/IndexCache/IndexCacheBase.hpp"

namespace Layer::Application::IndexCache {

class DeckForestSnapshotIndexCache final : public IndexCacheBase<DeckForestSnapshotIndexCacheDefinition> {
public:
    explicit DeckForestSnapshotIndexCache() noexcept;
};

}
