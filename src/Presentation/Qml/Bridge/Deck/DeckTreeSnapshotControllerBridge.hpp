#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>

#include "Bootstrap/RuntimeContext.hpp"
#include "Presentation/Controller/Deck/DeckTreeSnapshotController.hpp"

namespace Presentation::Qml::Bridge::Deck {

class DeckTreeSnapshotControllerBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::Deck::DeckTreeSnapshotModel* deckTreeSnapshot READ GetDeckTreeSnapshot CONSTANT)
    QML_NAMED_ELEMENT(DeckTreeSnapshotController)

public:
    explicit DeckTreeSnapshotControllerBridge(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckTreeSnapshotController{ Bootstrap::RuntimeContext::GetRequiredLibraryInvalidationChannel(),
                                        Bootstrap::RuntimeContext::GetRequiredDeckTreeSnapshotService() } {
    }

    ~DeckTreeSnapshotControllerBridge() noexcept override = default;
    DeckTreeSnapshotControllerBridge(const DeckTreeSnapshotControllerBridge&) = delete;
    DeckTreeSnapshotControllerBridge(DeckTreeSnapshotControllerBridge&&) = delete;
    DeckTreeSnapshotControllerBridge& operator=(const DeckTreeSnapshotControllerBridge&) = delete;
    DeckTreeSnapshotControllerBridge& operator=(DeckTreeSnapshotControllerBridge&&) = delete;

    [[nodiscard]] Presentation::Model::Deck::DeckTreeSnapshotModel* GetDeckTreeSnapshot() noexcept {
        return m_DeckTreeSnapshotController.GetDeckTreeSnapshot();
    }

private:
    Presentation::Controller::Deck::DeckTreeSnapshotController m_DeckTreeSnapshotController;
};

}
