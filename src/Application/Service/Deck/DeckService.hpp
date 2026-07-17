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
public:
    DeckService(Infrastructure::Store::Deck::DeckStore& DeckStore, Infrastructure::Store::Deck::DeckSnapshotStore& DeckSnapshotStore) noexcept
        : m_DeckStore{ DeckStore }
        , m_DeckSnapshotStore{ DeckSnapshotStore }
        , m_DeckForestSnapshotIndexCache{} {
    }

    DeckService(const DeckService&) = delete;
    DeckService(DeckService&&) = delete;
    DeckService& operator=(const DeckService&) = delete;
    DeckService& operator=(DeckService&&) = delete;

    [[nodiscard]] Application::IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease AcquireDeckForestSnapshotIndexCacheLease();

    [[nodiscard]] bool IsDeckNameLengthValid(const std::string&) const noexcept;

    void CreateRootDeck(const std::string&, std::uint8_t);
    void CreateChildDeck(const std::string&, const std::string&);
    void MoveDeck(const std::string&, const std::optional<std::string>&);
    void RenameDeck(const std::string&, const std::string&);
    void DeleteDeck(const std::string&);
    void RefreshDeckForestSnapshotIndexCache(const Application::IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease&, std::int64_t);

private:
    Infrastructure::Store::Deck::DeckStore& m_DeckStore;
    Infrastructure::Store::Deck::DeckSnapshotStore& m_DeckSnapshotStore;
    Application::IndexCache::Deck::DeckForestSnapshotIndexCache m_DeckForestSnapshotIndexCache;
};

}
