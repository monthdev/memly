#pragma once

#include <QObject>
#include <QtTypes>

#include "Application/Invalidation/LibraryInvalidationChannel.hpp"
#include "Presentation/Model/Deck/DeckTreeModel.hpp"

namespace Application::Service::Deck {
class DeckTreeService;
}

namespace Presentation::Controller::Deck {

class DeckTreeController final : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::Deck::DeckTreeModel* deckTree READ GetDeckTree CONSTANT)

public:
    explicit DeckTreeController(Application::Invalidation::LibraryInvalidationChannel& LibraryInvalidationChannel,
                                Application::Service::Deck::DeckTreeService& DeckTreeService,
                                QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_LibraryInvalidationChannel{ LibraryInvalidationChannel }
        , m_DeckTreeService{ DeckTreeService }
        , m_DeckTree{ this } {
        m_LibraryInvalidationChannel.ConnectSnapshot(
            this, Application::Invalidation::LibraryInvalidationTargetEnum::DeckTreeSnapshot, &DeckTreeController::RefreshDeckTree);
    }

    ~DeckTreeController() noexcept override = default;
    DeckTreeController(const DeckTreeController&) = delete;
    DeckTreeController(DeckTreeController&&) = delete;
    DeckTreeController& operator=(const DeckTreeController&) = delete;
    DeckTreeController& operator=(DeckTreeController&&) = delete;

    [[nodiscard]] Presentation::Model::Deck::DeckTreeModel* GetDeckTree() noexcept {
        return &m_DeckTree;
    }

private:
    Application::Invalidation::LibraryInvalidationChannel& m_LibraryInvalidationChannel;
    Application::Service::Deck::DeckTreeService& m_DeckTreeService;
    Presentation::Model::Deck::DeckTreeModel m_DeckTree;

    void RefreshDeckTree(const qint64) noexcept;
};
}
