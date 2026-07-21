#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "Application/IndexCache/Deck/DeckForestSnapshotIndexCache.hpp"

namespace Application::Domain::Deck::Index {
class DeckForestSnapshotIndex;
}

namespace Infrastructure::Store::Deck {
class DeckStore;
class DeckSnapshotStore;
}

namespace Application::Service::Deck {

class DeckService final {
private:
    Infrastructure::Store::Deck::DeckStore& m_DeckStore;
    Infrastructure::Store::Deck::DeckSnapshotStore& m_DeckSnapshotStore;
    Application::IndexCache::Deck::DeckForestSnapshotIndexCache m_DeckForestSnapshotIndexCache;

public:
    explicit DeckService(Infrastructure::Store::Deck::DeckStore& DeckStore, Infrastructure::Store::Deck::DeckSnapshotStore& DeckSnapshotStore) noexcept
        : m_DeckStore{ DeckStore }
        , m_DeckSnapshotStore{ DeckSnapshotStore }
        , m_DeckForestSnapshotIndexCache{} {
    }

    explicit DeckService(const DeckService&) = delete;
    explicit DeckService(DeckService&&) = delete;
    auto operator=(const DeckService&) -> DeckService& = delete;
    auto operator=(DeckService&&) -> DeckService& = delete;

    [[nodiscard]] auto AcquireDeckForestSnapshotIndexCacheLease() -> Application::IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease;

    [[nodiscard]] static auto IsDeckNameLengthValid(const std::string&) noexcept -> bool;

    void CreateRootDeck(const std::string&, std::uint8_t);
    void CreateChildDeck(const std::string&, const std::string&);
    void MoveDeck(const std::string&, const std::optional<std::string>&);
    void RenameDeck(const std::string&, const std::string&);
    void DeleteDeck(const std::string&);
    void RefreshDeckForestSnapshotIndexCache(const Application::IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease&, std::int64_t);
};

}
