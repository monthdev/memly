#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>
#include <QString>
#include <optional>

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
        , m_DeckTreeController{ Bootstrap::RuntimeContext::GetRequiredLibraryInvalidationChannel(),
                                Bootstrap::RuntimeContext::GetRequiredDeckService(),
                                Bootstrap::RuntimeContext::GetRequiredDeckTreeService() } {
    }

    ~DeckTreeControllerBridge() override = default;
    DeckTreeControllerBridge(const DeckTreeControllerBridge&) = delete;
    DeckTreeControllerBridge(DeckTreeControllerBridge&&) = delete;
    DeckTreeControllerBridge& operator=(const DeckTreeControllerBridge&) = delete;
    DeckTreeControllerBridge& operator=(DeckTreeControllerBridge&&) = delete;

    [[nodiscard]] Presentation::Model::Deck::DeckTreeModel* GetDeckTree() noexcept {
        return m_DeckTreeController.GetDeckTree();
    }

    [[nodiscard]] Q_INVOKABLE QString CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
        return m_DeckTreeController.CreateRootDeck(DeckName, TargetLanguageCode).value_or(QString{});
    }

    [[nodiscard]] Q_INVOKABLE QString CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
        return m_DeckTreeController.CreateChildDeck(DeckName, ParentDeckId).value_or(QString{});
    }

    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString& DeckId, const QString& NewParentDeckId = QString{}) noexcept {
        return m_DeckTreeController.MoveDeck(DeckId, NewParentDeckId.isEmpty() ? std::nullopt : std::make_optional(NewParentDeckId)).value_or(QString{});
    }

    [[nodiscard]] Q_INVOKABLE QString RenameDeck(const QString& DeckId, const QString& NewDeckName) noexcept {
        return m_DeckTreeController.RenameDeck(DeckId, NewDeckName).value_or(QString{});
    }

    [[nodiscard]] Q_INVOKABLE QString DeleteDeck(const QString& DeckId) noexcept {
        return m_DeckTreeController.DeleteDeck(DeckId).value_or(QString{});
    }

private:
    Presentation::Controller::Deck::DeckTreeController m_DeckTreeController;
};

}
