#include "Application/Service/Deck/DeckService.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "Application/Domain/Deck/Constraint/DeckConstraint.hpp"
#include "Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"
#include "Application/IndexCache/Deck/DeckForestSnapshotIndexCache.hpp"
#include "Infrastructure/Store/Deck/DeckSnapshotRecord.hpp"
#include "Infrastructure/Store/Deck/DeckSnapshotStore.hpp"
#include "Infrastructure/Store/Deck/DeckStore.hpp"

namespace Application::Service::Deck {

[[nodiscard]] auto DeckService::AcquireDeckForestSnapshotIndexCacheLease() -> Application::IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease {
    return m_DeckForestSnapshotIndexCache.AcquireLease();
}

[[nodiscard]] auto DeckService::IsDeckNameLengthValid(const std::string& DeckName) const noexcept -> bool {
    return Application::Domain::Deck::Constraint::IsDeckNameLengthValid(DeckName);
}

void DeckService::CreateRootDeck(const std::string& DeckName, const std::uint8_t TargetLanguageCode) {
    m_DeckStore.CreateRootDeck(DeckName, TargetLanguageCode);
}

void DeckService::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) {
    m_DeckStore.CreateChildDeck(DeckName, ParentDeckId);
}

void DeckService::MoveDeck(const std::string& DeckId, const std::optional<std::string>& NewParentDeckId) {
    m_DeckStore.MoveDeck(DeckId, NewParentDeckId);
}

void DeckService::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) {
    m_DeckStore.RenameDeck(DeckId, NewDeckName);
}

void DeckService::DeleteDeck(const std::string& DeckId) {
    m_DeckStore.DeleteDeck(DeckId);
}

void DeckService::RefreshDeckForestSnapshotIndexCache(
    const Application::IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease& DeckForestSnapshotIndexCacheLease,
    const std::int64_t AsOfMillisecondsSinceEpoch) {
    std::vector<Infrastructure::Store::Deck::DeckSnapshotRecord> DeckSnapshotRecordVector{ m_DeckSnapshotStore.ReadDeckSnapshotRecords(
        AsOfMillisecondsSinceEpoch) };
    std::vector<Application::Domain::Deck::Index::DeckForestSnapshotNode> DeckForestSnapshotNodeVector{};
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
    m_DeckForestSnapshotIndexCache.Refresh(DeckForestSnapshotIndexCacheLease, std::move(DeckForestSnapshotNodeVector));
}

}
