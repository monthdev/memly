#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>
#include <QString>
#include <QtTypes>
#include <optional>

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

    [[nodiscard]] Q_INVOKABLE QString CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
        return m_DeckController.CreateRootDeck(DeckName, TargetLanguageCode).value_or(QString{});
    }

    [[nodiscard]] Q_INVOKABLE QString CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
        return m_DeckController.CreateChildDeck(DeckName, ParentDeckId).value_or(QString{});
    }

    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString& DeckId, const QString& NewParentDeckId = QString{}) noexcept {
        return m_DeckController.MoveDeck(DeckId, NewParentDeckId.isEmpty() ? std::nullopt : std::make_optional(NewParentDeckId)).value_or(QString{});
    }

    [[nodiscard]] Q_INVOKABLE QString RenameDeck(const QString& DeckId, const QString& NewDeckName) noexcept {
        return m_DeckController.RenameDeck(DeckId, NewDeckName).value_or(QString{});
    }

    [[nodiscard]] Q_INVOKABLE QString DeleteDeck(const QString& DeckId) noexcept {
        return m_DeckController.DeleteDeck(DeckId).value_or(QString{});
    }

private:
    Presentation::Controller::Deck::DeckController m_DeckController;
};

}
