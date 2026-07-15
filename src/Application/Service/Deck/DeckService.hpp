#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "Application/IndexCache/Deck/DeckTreeSnapshotIndexCache.hpp"

namespace Application::Domain::Deck::Index {
class DeckTreeSnapshotIndex;
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
        , m_DeckTreeSnapshotIndexCache{} {
    }

    DeckService(const DeckService&) = delete;
    DeckService(DeckService&&) = delete;
    DeckService& operator=(const DeckService&) = delete;
    DeckService& operator=(DeckService&&) = delete;

    [[nodiscard]] Application::IndexCache::Deck::DeckTreeSnapshotIndexCache::IndexCacheLease AcquireDeckTreeSnapshotIndexCacheLease();

    [[nodiscard]] bool IsDeckNameLengthValid(const std::string&) const noexcept;

    void CreateRootDeck(const std::string&, std::uint8_t);
    void CreateChildDeck(const std::string&, const std::string&);
    void MoveDeck(const std::string&, const std::optional<std::string>&);
    void RenameDeck(const std::string&, const std::string&);
    void DeleteDeck(const std::string&);
    void RefreshDeckTreeSnapshotIndexCache(const Application::IndexCache::Deck::DeckTreeSnapshotIndexCache::IndexCacheLease&, std::int64_t);

private:
    Infrastructure::Store::Deck::DeckStore& m_DeckStore;
    Infrastructure::Store::Deck::DeckSnapshotStore& m_DeckSnapshotStore;
    Application::IndexCache::Deck::DeckTreeSnapshotIndexCache m_DeckTreeSnapshotIndexCache;
};

}
