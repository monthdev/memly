#include "Application/Service/Deck/DeckTreeService.hpp"

#include "Infrastructure/Store/Deck/DeckTreeStore.hpp"

namespace Application::Service::Deck {

DeckTreeService::DeckTreeService(Infrastructure::Store::Deck::DeckTreeStore& DeckTreeStore) noexcept
    : m_DeckTreeStore{ DeckTreeStore } {
}

[[nodiscard]] QVector<DeckTreeService::DeckTreeRow> DeckTreeService::ReadDeckTreeSnapshot(const qint64 AsOfMillisecondsSinceEpoch) {
    const QVector<Infrastructure::Store::Deck::DeckTreeStore::DeckTreeRow> StoreDeckTreeRowQVector{ m_DeckTreeStore.ReadDeckTreeSnapshot(
        AsOfMillisecondsSinceEpoch) };
    QVector<DeckTreeRow> DeckTreeRowQVector{};
    DeckTreeRowQVector.reserve(StoreDeckTreeRowQVector.size());
    for (const Infrastructure::Store::Deck::DeckTreeStore::DeckTreeRow& StoreDeckTreeRow : StoreDeckTreeRowQVector) {
        DeckTreeRowQVector.emplace_back(StoreDeckTreeRow.m_DeckId,
                                        StoreDeckTreeRow.m_ParentDeckId,
                                        StoreDeckTreeRow.m_DeckName,
                                        StoreDeckTreeRow.m_CreatedAtMillisecondsSinceEpoch,
                                        StoreDeckTreeRow.m_UpdatedAtMillisecondsSinceEpoch,
                                        StoreDeckTreeRow.m_SelfDueNowCount,
                                        StoreDeckTreeRow.m_SelfByTodayCount,
                                        StoreDeckTreeRow.m_SelfTotalCount,
                                        StoreDeckTreeRow.m_SubtreeDueNowCount,
                                        StoreDeckTreeRow.m_SubtreeByTodayCount,
                                        StoreDeckTreeRow.m_SubtreeTotalCount,
                                        StoreDeckTreeRow.m_TargetLanguageCode);
    }
    return DeckTreeRowQVector;
}

}
