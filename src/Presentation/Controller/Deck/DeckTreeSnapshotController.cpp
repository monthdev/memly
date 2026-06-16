#include "Presentation/Controller/Deck/DeckTreeSnapshotController.hpp"

#include <cstdint>
#include <utility>
#include <vector>

#include "Application/Service/Deck/DeckTreeSnapshotService.hpp"
#include "Domain/Deck/DeckTreeSnapshotNode.hpp"
#include "Runtime/Crash.hpp"

namespace Presentation::Controller::Deck {

void DeckTreeSnapshotController::RefreshDeckTreeSnapshot(const std::int64_t AsOfMillisecondsSinceEpoch) noexcept {
    Runtime::TryCatchWrapper([&]() -> void {
        std::vector<Domain::Deck::DeckTreeSnapshotNode> DeckTreeSnapshotNodeVector{ m_DeckTreeSnapshotService.ReadDeckTreeSnapshotNodes(
            AsOfMillisecondsSinceEpoch) };
        m_DeckTreeSnapshot.ReplaceAll(std::move(DeckTreeSnapshotNodeVector));
    });
}
}
