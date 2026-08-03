#include "Layer/Application/Service/Deck/DeckService.hpp"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "Layer/Application/Domain/Deck/Constraint/DeckConstraint.hpp"
#include "Layer/Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"
#include "Layer/Application/IndexCache/Deck/DeckForestSnapshotIndexCache.hpp"
#include "Layer/Infrastructure/Store/Deck/DeckSnapshotRecord.hpp"
#include "Layer/Infrastructure/Store/Deck/DeckSnapshotStore.hpp"
#include "Layer/Infrastructure/Store/Deck/DeckStore.hpp"

namespace Layer::Application::Service::Deck {

[[nodiscard]] auto DeckService::AcquireDeckForestSnapshotIndexCacheLease() -> IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease {
    return IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease{ this->m_DeckForestSnapshotIndexCache.AcquireLease() };
}

[[nodiscard]] auto DeckService::IsDeckNameLengthValid(const std::string& DeckName) noexcept -> bool {
    return bool{ Domain::Deck::Constraint::IsDeckNameLengthValid(DeckName) };
}

void DeckService::CreateRootDeck(const std::string& DeckName, const std::uint8_t TargetLanguageCode) {
    this->m_DeckStore.CreateRootDeck(DeckName, TargetLanguageCode);
}

void DeckService::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) {
    this->m_DeckStore.CreateChildDeck(DeckName, ParentDeckId);
}

void DeckService::MoveDeckToRoot(const std::string& DeckId) {
    this->m_DeckStore.MoveDeckToRoot(DeckId);
}

void DeckService::MoveDeckUnderParent(const std::string& DeckId, const std::string& ParentDeckId) {
    this->m_DeckStore.MoveDeckUnderParent(DeckId, ParentDeckId);
}

void DeckService::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) {
    this->m_DeckStore.RenameDeck(DeckId, NewDeckName);
}

void DeckService::DeleteDeck(const std::string& DeckId) {
    this->m_DeckStore.DeleteDeck(DeckId);
}

void DeckService::RefreshDeckForestSnapshotIndexCache(const IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease& DeckForestSnapshotIndexCacheLease,
                                                      const std::int64_t AsOfMillisecondsSinceEpoch) {
    std::vector<Infrastructure::Store::Deck::DeckSnapshotRecord> DeckSnapshotRecordVector{ this->m_DeckSnapshotStore.ReadDeckSnapshotRecords(
        AsOfMillisecondsSinceEpoch) };
    std::vector<Domain::Deck::Index::DeckForestSnapshotNode> DeckForestSnapshotNodeVector{};
    DeckForestSnapshotNodeVector.reserve(DeckSnapshotRecordVector.size());
    for (Infrastructure::Store::Deck::DeckSnapshotRecord& DeckSnapshotRecord : DeckSnapshotRecordVector) {
        DeckForestSnapshotNodeVector.emplace_back(std::move(DeckSnapshotRecord.m_DeckId),
                                                  std::move(DeckSnapshotRecord.m_ParentDeckIdOptional),
                                                  std::move(DeckSnapshotRecord.m_DeckName),
                                                  DeckSnapshotRecord.m_CreatedAtMillisecondsSinceEpoch,
                                                  DeckSnapshotRecord.m_LastUpdatedAtMillisecondsSinceEpochOptional,
                                                  DeckSnapshotRecord.m_SelfDueNowCount,
                                                  DeckSnapshotRecord.m_SelfByTodayCount,
                                                  DeckSnapshotRecord.m_SelfTotalCount,
                                                  DeckSnapshotRecord.m_TargetLanguageCode);
    }
    this->m_DeckForestSnapshotIndexCache.Refresh(DeckForestSnapshotIndexCacheLease, std::move(DeckForestSnapshotNodeVector));
}

}
