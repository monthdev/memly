#include "Application/Service/Deck/DeckService.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Application/Domain/Deck/Constraint/DeckConstraint.hpp"
#include "Application/Domain/Deck/Index/DeckTreeSnapshotIndex.hpp"
#include "Infrastructure/Store/Deck/DeckSnapshotStore.hpp"
#include "Infrastructure/Store/Deck/DeckStore.hpp"

namespace Application::Service::Deck {

[[nodiscard]] Application::IndexCache::Deck::DeckTreeSnapshotIndexCache::IndexCacheLease DeckService::AcquireDeckTreeSnapshotIndexCacheLease() {
    return m_DeckTreeSnapshotIndexCache.AcquireLease();
}

[[nodiscard]] bool DeckService::IsDeckNameLengthValid(const std::string_view DeckName) const noexcept {
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

void DeckService::RefreshDeckTreeSnapshotIndexCache(
    const Application::IndexCache::Deck::DeckTreeSnapshotIndexCache::IndexCacheLease& DeckTreeSnapshotIndexCacheLease,
    const std::int64_t AsOfMillisecondsSinceEpoch) {
    std::vector<Infrastructure::Store::Deck::DeckSnapshotStore::DeckSnapshotRecord> DeckSnapshotRecordVector{ m_DeckSnapshotStore.ReadDeckSnapshotRecords(
        AsOfMillisecondsSinceEpoch) };
    std::vector<Application::Domain::Deck::Index::DeckTreeSnapshotIndex::DeckTreeSnapshotNode> DeckTreeSnapshotNodeVector{};
    DeckTreeSnapshotNodeVector.reserve(DeckSnapshotRecordVector.size());
    for (Infrastructure::Store::Deck::DeckSnapshotStore::DeckSnapshotRecord& DeckSnapshotRecord : DeckSnapshotRecordVector) {
        DeckTreeSnapshotNodeVector.emplace_back(std::move(DeckSnapshotRecord.m_DeckId),
                                                std::move(DeckSnapshotRecord.m_ParentDeckIdOptional),
                                                std::move(DeckSnapshotRecord.m_DeckName),
                                                DeckSnapshotRecord.m_CreatedAtMillisecondsSinceEpoch,
                                                std::move(DeckSnapshotRecord.m_LastUpdatedAtMillisecondsSinceEpochOptional),
                                                DeckSnapshotRecord.m_SelfDueNowCount,
                                                DeckSnapshotRecord.m_SelfByTodayCount,
                                                DeckSnapshotRecord.m_SelfTotalCount,
                                                DeckSnapshotRecord.m_TargetLanguageCode);
    }
    m_DeckTreeSnapshotIndexCache.Refresh(DeckTreeSnapshotIndexCacheLease, std::move(DeckTreeSnapshotNodeVector));
}

}
