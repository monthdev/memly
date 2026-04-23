#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>

#include "App/Bootstrap/AppContext.hpp"
#include "Presentation/Controller/DeckTreeTableController.hpp"

namespace Presentation::Qml::Bridge {

class DeckTreeTableControllerQml : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::DeckTreeTableModel* deckTreeTable READ GetDeckTreeTable CONSTANT)
    QML_NAMED_ELEMENT(DeckTreeTableController)

public:
    explicit DeckTreeTableControllerQml(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckTreeTableController{ App::Bootstrap::AppContext::GetRequiredDeckHierarchyStore(), App::Bootstrap::AppContext::GetRequiredDeckStore() } {
    }

    [[nodiscard]] Model::DeckTreeTableModel* GetDeckTreeTable() noexcept {
        return m_DeckTreeTableController.GetDeckTreeTable();
    }

    [[nodiscard]] Q_INVOKABLE QString CreateDeck(const QString& DeckName, const quint8 TargetLanguageCode, const QString& ParentDeckId = QString{}) noexcept {
        return m_DeckTreeTableController.CreateDeck(DeckName, TargetLanguageCode, ParentDeckId);
    }

    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString& DeckId, const QString& NewParentDeckId = QString{}) noexcept {
        return m_DeckTreeTableController.MoveDeck(DeckId, NewParentDeckId);
    }

    [[nodiscard]] Q_INVOKABLE QString UpdateDeckName(const QString& DeckId, const QString& DeckName) noexcept {
        return m_DeckTreeTableController.UpdateDeckName(DeckId, DeckName);
    }

    Q_INVOKABLE void DeleteDeck(const QString& DeckId) noexcept {
        m_DeckTreeTableController.DeleteDeck(DeckId);
    }

    Q_INVOKABLE void OnActivated() noexcept {
        m_DeckTreeTableController.OnActivated();
    }

    Q_INVOKABLE void OnDeactivated() {
        m_DeckTreeTableController.OnDeactivated();
    }

private:
    Controller::DeckTreeTableController m_DeckTreeTableController;
};

}
