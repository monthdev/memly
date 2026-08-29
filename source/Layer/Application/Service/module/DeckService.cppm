module;

#include <cstdint>
#include <string>

export module Memly.Service.DeckService;

import Memly.IndexCache.DeckForestSnapshotIndexCache;
import Memly.Repository.DeckRepository;

export namespace Memly::Service {

class DeckService final {
private:
    Repository::DeckRepository& m_DeckRepository;
    IndexCache::DeckForestSnapshotIndexCache m_DeckForestSnapshotIndexCache;

public:
    explicit DeckService(
        [[clang::lifetimebound]] Repository::DeckRepository&
    ) noexcept;

    explicit DeckService(const DeckService&) = delete;
    DeckService&
    operator=(const DeckService&) = delete;

    explicit DeckService(DeckService&&) = delete;
    DeckService&
    operator=(DeckService&&) = delete;

    ~DeckService() noexcept = default;

    [[nodiscard]] IndexCache::DeckForestSnapshotIndexCache::IndexCacheLease
    AcquireDeckForestSnapshotIndexCacheLease();

    [[nodiscard]] static bool
    IsDeckNameLengthValid(const std::string&) noexcept;

    void
    CreateRootDeck(const std::string&, std::uint8_t);

    void
    CreateChildDeck(const std::string&, const std::string&);

    void
    MoveDeckToRoot(const std::string&);

    void
    MoveDeckUnderParent(const std::string&, const std::string&);

    void
    RenameDeck(const std::string&, const std::string&);

    void
    DeleteDeck(const std::string&);

    void
    RefreshDeckForestSnapshotIndexCache(
        const IndexCache::DeckForestSnapshotIndexCache::IndexCacheLease&,
        std::int64_t
    );
};

}
