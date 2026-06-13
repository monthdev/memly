#include "Application/Service/Deck/DeckTreeSnapshotService.hpp"

#include "Infrastructure/Store/Deck/DeckTreeSnapshotStore.hpp"

namespace Application::Service::Deck {

[[nodiscard]] std::vector<Domain::Deck::DeckTreeSnapshotNodeData> DeckTreeSnapshotService::ReadDeckTreeSnapshot(const qint64 AsOfMillisecondsSinceEpoch) {
    return m_DeckTreeSnapshotStore.ReadDeckTreeSnapshot(AsOfMillisecondsSinceEpoch);
}

}
