#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>
#include <QString>
#include <optional>

#include "Bootstrap/RuntimeContext.hpp"
#include "Presentation/Controller/DeckTreeController.hpp"

namespace Presentation::Qml::Bridge {

class DeckTreeControllerQml : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::DeckTreeModel* deckTree READ GetDeckTree CONSTANT)
    QML_NAMED_ELEMENT(DeckTreeController)

public:
    explicit DeckTreeControllerQml(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckTreeController{ Bootstrap::RuntimeContext::GetRequiredLibraryRefreshCoordinator(),
                                Bootstrap::RuntimeContext::GetRequiredDeckService(),
                                Bootstrap::RuntimeContext::GetRequiredDeckTreeService() } {
    }

    [[nodiscard]] Model::DeckTreeModel* GetDeckTree() noexcept {
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
    Controller::DeckTreeController m_DeckTreeController;
};

}
