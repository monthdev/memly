#include "Presentation/Controller/Deck/DeckTreeSnapshotController.hpp"

#include "Application/Service/Deck/DeckTreeSnapshotService.hpp"
#include "Runtime/Crash.hpp"

namespace Presentation::Controller::Deck {

void DeckTreeSnapshotController::RefreshDeckTreeSnapshot(const qint64 AsOfMillisecondsSinceEpoch) noexcept {
    Runtime::TryCatchWrapper([&]() -> void {
        const QVector<Application::Service::Deck::DeckTreeSnapshotService::DeckTreeSnapshotRow> DeckTreeSnapshotRowQVector{
            m_DeckTreeSnapshotService.ReadDeckTreeSnapshot(AsOfMillisecondsSinceEpoch)
        };
        QVector<Presentation::Model::Deck::DeckTreeSnapshotModel::DeckNodeData> DeckNodeDataQVector{};
        DeckNodeDataQVector.reserve(DeckTreeSnapshotRowQVector.size());
        for (const Application::Service::Deck::DeckTreeSnapshotService::DeckTreeSnapshotRow& DeckTreeSnapshotRow : DeckTreeSnapshotRowQVector) {
            DeckNodeDataQVector.emplace_back(DeckTreeSnapshotRow.m_DeckId,
                                             DeckTreeSnapshotRow.m_ParentDeckId,
                                             DeckTreeSnapshotRow.m_DeckName,
                                             DeckTreeSnapshotRow.m_CreatedAtMillisecondsSinceEpoch,
                                             DeckTreeSnapshotRow.m_UpdatedAtMillisecondsSinceEpoch,
                                             DeckTreeSnapshotRow.m_SelfDueNowCount,
                                             DeckTreeSnapshotRow.m_SelfByTodayCount,
                                             DeckTreeSnapshotRow.m_SelfTotalCount,
                                             DeckTreeSnapshotRow.m_SubtreeDueNowCount,
                                             DeckTreeSnapshotRow.m_SubtreeByTodayCount,
                                             DeckTreeSnapshotRow.m_SubtreeTotalCount,
                                             DeckTreeSnapshotRow.m_TargetLanguageCode);
        }
        m_DeckTreeSnapshot.ReplaceAll(std::move(DeckNodeDataQVector));
    });
}
}
