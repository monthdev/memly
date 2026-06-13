#pragma once

#include <QObject>
#include <QtTypes>

#include "Application/Invalidation/LibraryInvalidationChannel.hpp"
#include "Presentation/Model/Deck/DeckTreeSnapshotModel.hpp"

namespace Application::Service::Deck {
class DeckTreeSnapshotService;
}

namespace Presentation::Controller::Deck {

class DeckTreeSnapshotController final : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::Deck::DeckTreeSnapshotModel* deckTreeSnapshot READ GetDeckTreeSnapshot CONSTANT)

public:
    explicit DeckTreeSnapshotController(Application::Invalidation::LibraryInvalidationChannel& LibraryInvalidationChannel,
                                        Application::Service::Deck::DeckTreeSnapshotService& DeckTreeSnapshotService,
                                        QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_LibraryInvalidationChannel{ LibraryInvalidationChannel }
        , m_DeckTreeSnapshotService{ DeckTreeSnapshotService }
        , m_DeckTreeSnapshot{ this } {
        m_LibraryInvalidationChannel.ConnectSnapshot(
            this, Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot, &DeckTreeSnapshotController::RefreshDeckTreeSnapshot);
    }

    ~DeckTreeSnapshotController() noexcept override = default;
    DeckTreeSnapshotController(const DeckTreeSnapshotController&) = delete;
    DeckTreeSnapshotController(DeckTreeSnapshotController&&) = delete;
    DeckTreeSnapshotController& operator=(const DeckTreeSnapshotController&) = delete;
    DeckTreeSnapshotController& operator=(DeckTreeSnapshotController&&) = delete;

    [[nodiscard]] Presentation::Model::Deck::DeckTreeSnapshotModel* GetDeckTreeSnapshot() noexcept {
        return &m_DeckTreeSnapshot;
    }

private:
    Application::Invalidation::LibraryInvalidationChannel& m_LibraryInvalidationChannel;
    Application::Service::Deck::DeckTreeSnapshotService& m_DeckTreeSnapshotService;
    Presentation::Model::Deck::DeckTreeSnapshotModel m_DeckTreeSnapshot;

    void RefreshDeckTreeSnapshot(const qint64) noexcept;
};
}
