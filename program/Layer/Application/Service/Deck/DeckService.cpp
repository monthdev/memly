#include "Layer/Application/Service/Deck/DeckService.hpp"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "Layer/Application/Domain/Deck/Constraint/DeckConstraint.hpp"
#include "Layer/Application/Domain/Deck/Index/DeckForestSnapshotNode.hpp"
#include "Layer/Application/IndexCache/Deck/DeckForestSnapshotIndexCache.hpp"
#include "Layer/Infrastructure/DuckDb/Repository/Deck/DeckRepository.hpp"
#include "Layer/Infrastructure/DuckDb/Repository/Deck/DeckSnapshotRecord.hpp"
#include "Support/SpecialMemberPolicy/NoCopyNoMoveMixin.hpp"

namespace Layer::Application::Service::Deck {

DeckService::DeckService(Infrastructure::DuckDb::Repository::Deck::DeckRepository& DeckRepository) noexcept
    : Support::SpecialMemberPolicy::NoCopyNoMoveMixin{}
    , m_DeckRepository{ DeckRepository }
    , m_DeckForestSnapshotIndexCache{} {
}

[[nodiscard]] auto DeckService::AcquireDeckForestSnapshotIndexCacheLease() -> IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease {
    return IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease{ this->m_DeckForestSnapshotIndexCache.AcquireLease() };
}

[[nodiscard]] auto DeckService::IsDeckNameLengthValid(const std::string& DeckName) noexcept -> bool {
    return bool{ Domain::Deck::Constraint::IsDeckNameLengthValid(DeckName) };
}

void DeckService::CreateRootDeck(const std::string& DeckName, const std::uint8_t TargetLanguageCode) {
    this->m_DeckRepository.CreateRootDeck(DeckName, TargetLanguageCode);
}

void DeckService::CreateChildDeck(const std::string& DeckName, const std::string& ParentDeckId) {
    this->m_DeckRepository.CreateChildDeck(DeckName, ParentDeckId);
}

void DeckService::MoveDeckToRoot(const std::string& DeckId) {
    this->m_DeckRepository.MoveDeckToRoot(DeckId);
}

void DeckService::MoveDeckUnderParent(const std::string& DeckId, const std::string& ParentDeckId) {
    this->m_DeckRepository.MoveDeckUnderParent(DeckId, ParentDeckId);
}

void DeckService::RenameDeck(const std::string& DeckId, const std::string& NewDeckName) {
    this->m_DeckRepository.RenameDeck(DeckId, NewDeckName);
}

void DeckService::DeleteDeck(const std::string& DeckId) {
    this->m_DeckRepository.DeleteDeck(DeckId);
}

void DeckService::RefreshDeckForestSnapshotIndexCache(const IndexCache::Deck::DeckForestSnapshotIndexCache::IndexCacheLease& DeckForestSnapshotIndexCacheLease,
                                                      const std::int64_t AsOfMillisecondsSinceEpoch) {
    std::vector<Infrastructure::DuckDb::Repository::Deck::DeckSnapshotRecord> DeckSnapshotRecordVector{ this->m_DeckRepository.ReadDeckSnapshotRecords(
        AsOfMillisecondsSinceEpoch) };
    std::vector<Domain::Deck::Index::DeckForestSnapshotNode> DeckForestSnapshotNodeVector{};
    DeckForestSnapshotNodeVector.reserve(DeckSnapshotRecordVector.size());
    for (Infrastructure::DuckDb::Repository::Deck::DeckSnapshotRecord& DeckSnapshotRecord : DeckSnapshotRecordVector) {
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
