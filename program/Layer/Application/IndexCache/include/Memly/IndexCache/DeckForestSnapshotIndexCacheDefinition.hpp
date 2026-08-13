#pragma once

#include <vector>

#include "Memly/Domain/DeckForestSnapshotIndex.hpp"
#include "Memly/IndexCache/IndexCacheDefinition.hpp"

namespace Layer::Application::Domain {
struct DeckForestSnapshotNode;
}

namespace Layer::Application::IndexCache {

class DeckForestSnapshotIndexCacheDefinition final : public IndexCacheDefinition<Domain::DeckForestSnapshotIndex,
                                                                                 std::vector<Domain::DeckForestSnapshotNode>,
                                                                                 &Domain::DeckForestSnapshotIndex::RefreshFromDeckForestSnapshotNodes> {
public:
    explicit DeckForestSnapshotIndexCacheDefinition() = delete;
};

}
