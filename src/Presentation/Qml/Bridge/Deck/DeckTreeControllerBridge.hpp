#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>

#include "Bootstrap/RuntimeContext.hpp"
#include "Presentation/Controller/Deck/DeckTreeController.hpp"

namespace Presentation::Qml::Bridge::Deck {

class DeckTreeControllerBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::Deck::DeckTreeModel* deckTree READ GetDeckTree CONSTANT)
    QML_NAMED_ELEMENT(DeckTreeController)

public:
    explicit DeckTreeControllerBridge(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckTreeController{ Bootstrap::RuntimeContext::GetRequiredLibraryInvalidationChannel(), Bootstrap::RuntimeContext::GetRequiredDeckTreeService() } {
    }

    ~DeckTreeControllerBridge() noexcept override = default;
    DeckTreeControllerBridge(const DeckTreeControllerBridge&) = delete;
    DeckTreeControllerBridge(DeckTreeControllerBridge&&) = delete;
    DeckTreeControllerBridge& operator=(const DeckTreeControllerBridge&) = delete;
    DeckTreeControllerBridge& operator=(DeckTreeControllerBridge&&) = delete;

    [[nodiscard]] Presentation::Model::Deck::DeckTreeModel* GetDeckTree() noexcept {
        return m_DeckTreeController.GetDeckTree();
    }

private:
    Presentation::Controller::Deck::DeckTreeController m_DeckTreeController;
};

}
