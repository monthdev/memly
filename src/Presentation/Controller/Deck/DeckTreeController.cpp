#include "Presentation/Controller/Deck/DeckTreeController.hpp"

#include "Application/Service/Deck/DeckTreeService.hpp"
#include "Runtime/Crash.hpp"

namespace Presentation::Controller::Deck {

void DeckTreeController::RefreshDeckTree(const qint64 AsOfMillisecondsSinceEpoch) noexcept {
    Runtime::TryCatchWrapper([&]() -> void {
        const QVector<Application::Service::Deck::DeckTreeService::DeckTreeRow> DeckTreeRowQVector{ m_DeckTreeService.ReadDeckTreeSnapshot(
            AsOfMillisecondsSinceEpoch) };
        QVector<Presentation::Model::Deck::DeckTreeModel::DeckNodeData> DeckNodeDataQVector{};
        DeckNodeDataQVector.reserve(DeckTreeRowQVector.size());
        for (const Application::Service::Deck::DeckTreeService::DeckTreeRow& DeckTreeRow : DeckTreeRowQVector) {
            DeckNodeDataQVector.emplace_back(DeckTreeRow.m_DeckId,
                                             DeckTreeRow.m_ParentDeckId,
                                             DeckTreeRow.m_DeckName,
                                             DeckTreeRow.m_CreatedAtMillisecondsSinceEpoch,
                                             DeckTreeRow.m_UpdatedAtMillisecondsSinceEpoch,
                                             DeckTreeRow.m_SelfDueNowCount,
                                             DeckTreeRow.m_SelfByTodayCount,
                                             DeckTreeRow.m_SelfTotalCount,
                                             DeckTreeRow.m_SubtreeDueNowCount,
                                             DeckTreeRow.m_SubtreeByTodayCount,
                                             DeckTreeRow.m_SubtreeTotalCount,
                                             DeckTreeRow.m_TargetLanguageCode);
        }
        m_DeckTree.ReplaceAll(std::move(DeckNodeDataQVector));
    });
}
}
