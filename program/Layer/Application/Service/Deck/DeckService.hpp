#pragma once

#include <cstdint>
#include <string>

#include "Layer/Application/IndexCache/Deck/DeckForestSnapshotIndexCache.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Application::Domain::Deck::Index {
class DeckForestSnapshotIndex;
}

namespace Layer::Infrastructure::DuckDb::Repository::Deck {
class DeckRepository;
}

namespace Layer::Application::Service::Deck {

class DeckService final : private Support::SpecialMemberPolicy::NoCopyNoMoveMixin {
private:
    Infrastructure::DuckDb::Repository::Deck::DeckRepository& m_DeckRepository;
    IndexCache::Deck::DeckForestSnapshotIndexCache m_DeckForestSnapshotIndexCache;

public:
    explicit DeckService(Infrastructure::DuckDb::Repository::Deck::DeckRepository&) noexcept;

    [[nodiscard]] auto AcquireDeckForestSnapshotIndexCacheLease() -> IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease;

    [[nodiscard]] static auto IsDeckNameLengthValid(const std::string&) noexcept -> bool;

    void CreateRootDeck(const std::string&, std::uint8_t);
    void CreateChildDeck(const std::string&, const std::string&);
    void MoveDeckToRoot(const std::string&);
    void MoveDeckUnderParent(const std::string&, const std::string&);
    void RenameDeck(const std::string&, const std::string&);
    void DeleteDeck(const std::string&);
    void RefreshDeckForestSnapshotIndexCache(const IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease&, std::int64_t);
};

}
