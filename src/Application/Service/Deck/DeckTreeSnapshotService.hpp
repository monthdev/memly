#pragma once

#include <QtTypes>
#include <vector>

#include "Domain/Deck/DeckTreeSnapshotNodeData.hpp"

namespace Infrastructure::Store::Deck {
class DeckTreeSnapshotStore;
}

namespace Application::Service::Deck {

class DeckTreeSnapshotService final {
public:
    explicit DeckTreeSnapshotService(Infrastructure::Store::Deck::DeckTreeSnapshotStore& DeckTreeSnapshotStore) noexcept
        : m_DeckTreeSnapshotStore{ DeckTreeSnapshotStore } {
    }

    ~DeckTreeSnapshotService() noexcept = default;
    DeckTreeSnapshotService(const DeckTreeSnapshotService&) = delete;
    DeckTreeSnapshotService(DeckTreeSnapshotService&&) = delete;
    DeckTreeSnapshotService& operator=(const DeckTreeSnapshotService&) = delete;
    DeckTreeSnapshotService& operator=(DeckTreeSnapshotService&&) = delete;

    [[nodiscard]] std::vector<Domain::Deck::DeckTreeSnapshotNodeData> ReadDeckTreeSnapshot(qint64);

private:
    Infrastructure::Store::Deck::DeckTreeSnapshotStore& m_DeckTreeSnapshotStore;
};

}
