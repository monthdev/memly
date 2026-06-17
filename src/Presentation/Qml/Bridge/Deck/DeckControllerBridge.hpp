#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>
#include <QString>
#include <QtTypes>

#include "Bootstrap/RuntimeContext.hpp"
#include "Presentation/Controller/Deck/DeckController.hpp"

namespace Presentation::Qml::Bridge::Deck {

class DeckControllerBridge : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(DeckController)

public:
    explicit DeckControllerBridge(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckController{ Bootstrap::RuntimeContext::GetRequiredDeckService() } {
    }

    ~DeckControllerBridge() noexcept override = default;
    DeckControllerBridge(const DeckControllerBridge&) = delete;
    DeckControllerBridge(DeckControllerBridge&&) = delete;
    DeckControllerBridge& operator=(const DeckControllerBridge&) = delete;
    DeckControllerBridge& operator=(DeckControllerBridge&&) = delete;

    [[nodiscard]] Q_INVOKABLE QString CreateRootDeck(const QString&, quint8) noexcept;
    [[nodiscard]] Q_INVOKABLE QString CreateChildDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString&, const QString& = QString{}) noexcept;
    [[nodiscard]] Q_INVOKABLE QString RenameDeck(const QString&, const QString&) noexcept;
    [[nodiscard]] Q_INVOKABLE QString DeleteDeck(const QString&) noexcept;

private:
    Presentation::Controller::Deck::DeckController m_DeckController;
};

}
