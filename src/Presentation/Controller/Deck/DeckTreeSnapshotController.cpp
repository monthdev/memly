#include "Presentation/Controller/Deck/DeckTreeSnapshotController.hpp"

#include <utility>
#include <vector>

#include "Application/Service/Deck/DeckTreeSnapshotService.hpp"
#include "Domain/Deck/DeckTreeSnapshotNodeData.hpp"
#include "Runtime/Crash.hpp"

namespace Presentation::Controller::Deck {

void DeckTreeSnapshotController::RefreshDeckTreeSnapshot(const qint64 AsOfMillisecondsSinceEpoch) noexcept {
    Runtime::TryCatchWrapper([&]() -> void {
        std::vector<Domain::Deck::DeckTreeSnapshotNodeData> DeckTreeSnapshotNodeDataVector{ m_DeckTreeSnapshotService.ReadDeckTreeSnapshot(
            AsOfMillisecondsSinceEpoch) };
        m_DeckTreeSnapshot.ReplaceAll(std::move(DeckTreeSnapshotNodeDataVector));
    });
}
}
