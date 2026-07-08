#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include "Application/IndexCache/Deck/DeckTreeSnapshotIndexCache.hpp"
#include "Application/IndexCache/Deck/DeckTreeSnapshotIndexCacheLease.hpp"

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

    ~DeckService() noexcept = default;
    DeckService(const DeckService&) = delete;
    DeckService(DeckService&&) = delete;
    DeckService& operator=(const DeckService&) = delete;
    DeckService& operator=(DeckService&&) = delete;

    [[nodiscard]] Application::IndexCache::Deck::DeckTreeSnapshotIndexCacheLease AcquireDeckTreeSnapshotIndexCacheLease();
    [[nodiscard]] const Application::Domain::Deck::Index::DeckTreeSnapshotIndex&
    GetDeckTreeSnapshotIndex(const Application::IndexCache::Deck::DeckTreeSnapshotIndexCacheLease&) const noexcept;

    [[nodiscard]] bool IsDeckNameLengthValid(const std::string_view) const noexcept;

    void CreateRootDeck(const std::string&, std::uint8_t);
    void CreateChildDeck(const std::string&, const std::string&);
    void MoveDeck(const std::string&, const std::optional<std::string>&);
    void RenameDeck(const std::string&, const std::string&);
    void DeleteDeck(const std::string&);
    void RefreshDeckTreeSnapshotIndexCache(const Application::IndexCache::Deck::DeckTreeSnapshotIndexCacheLease&, std::int64_t);

private:
    Infrastructure::Store::Deck::DeckStore& m_DeckStore;
    Infrastructure::Store::Deck::DeckSnapshotStore& m_DeckSnapshotStore;
    Application::IndexCache::Deck::DeckTreeSnapshotIndexCache m_DeckTreeSnapshotIndexCache;
};

}
