#include "Application/Service/Deck/DeckTreeSnapshotService.hpp"

#include "Infrastructure/Store/Deck/DeckTreeSnapshotStore.hpp"

namespace Application::Service::Deck {

[[nodiscard]] std::vector<Domain::Deck::DeckTreeSnapshotNode> DeckTreeSnapshotService::ReadDeckTreeSnapshot(const std::int64_t AsOfMillisecondsSinceEpoch) {
    return m_DeckTreeSnapshotStore.ReadDeckTreeSnapshot(AsOfMillisecondsSinceEpoch);
}

}
