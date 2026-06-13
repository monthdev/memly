#include "Application/Service/Deck/DeckTreeSnapshotService.hpp"

#include "Infrastructure/Store/Deck/DeckTreeSnapshotStore.hpp"

namespace Application::Service::Deck {

[[nodiscard]] QVector<DeckTreeSnapshotService::DeckTreeSnapshotRow> DeckTreeSnapshotService::ReadDeckTreeSnapshot(const qint64 AsOfMillisecondsSinceEpoch) {
    const QVector<Infrastructure::Store::Deck::DeckTreeSnapshotStore::DeckTreeSnapshotRow> StoreDeckTreeSnapshotRowQVector{
        m_DeckTreeSnapshotStore.ReadDeckTreeSnapshot(AsOfMillisecondsSinceEpoch)
    };
    QVector<DeckTreeSnapshotRow> DeckTreeSnapshotRowQVector{};
    DeckTreeSnapshotRowQVector.reserve(StoreDeckTreeSnapshotRowQVector.size());
    for (const Infrastructure::Store::Deck::DeckTreeSnapshotStore::DeckTreeSnapshotRow& StoreDeckTreeSnapshotRow : StoreDeckTreeSnapshotRowQVector) {
        DeckTreeSnapshotRowQVector.emplace_back(StoreDeckTreeSnapshotRow.m_DeckId,
                                                StoreDeckTreeSnapshotRow.m_ParentDeckId,
                                                StoreDeckTreeSnapshotRow.m_DeckName,
                                                StoreDeckTreeSnapshotRow.m_CreatedAtMillisecondsSinceEpoch,
                                                StoreDeckTreeSnapshotRow.m_UpdatedAtMillisecondsSinceEpoch,
                                                StoreDeckTreeSnapshotRow.m_SelfDueNowCount,
                                                StoreDeckTreeSnapshotRow.m_SelfByTodayCount,
                                                StoreDeckTreeSnapshotRow.m_SelfTotalCount,
                                                StoreDeckTreeSnapshotRow.m_SubtreeDueNowCount,
                                                StoreDeckTreeSnapshotRow.m_SubtreeByTodayCount,
                                                StoreDeckTreeSnapshotRow.m_SubtreeTotalCount,
                                                StoreDeckTreeSnapshotRow.m_TargetLanguageCode);
    }
    return DeckTreeSnapshotRowQVector;
}

}
