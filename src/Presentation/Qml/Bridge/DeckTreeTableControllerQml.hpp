#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>

#include "Bootstrap/AppContext.hpp"
#include "Presentation/Controller/DeckTreeTableController.hpp"

namespace Presentation::Qml::Bridge {

class DeckTreeTableControllerQml : public QObject {
    Q_OBJECT
    Q_PROPERTY(Presentation::Model::DeckTreeTableModel* deckTreeTable READ GetDeckTreeTable CONSTANT)
    QML_NAMED_ELEMENT(DeckTreeTableController)

public:
    explicit DeckTreeTableControllerQml(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckTreeTableController{ Bootstrap::AppContext::GetRequiredDeckHierarchyStore(), Bootstrap::AppContext::GetRequiredDeckStore() } {
    }

    [[nodiscard]] Model::DeckTreeTableModel* GetDeckTreeTable() noexcept {
        return m_DeckTreeTableController.GetDeckTreeTable();
    }

    [[nodiscard]] Q_INVOKABLE QString CreateRootDeck(const QString& DeckName, const quint8 TargetLanguageCode) noexcept {
        return m_DeckTreeTableController.CreateRootDeck(DeckName, TargetLanguageCode);
    }

    [[nodiscard]] Q_INVOKABLE QString CreateChildDeck(const QString& DeckName, const QString& ParentDeckId) noexcept {
        return m_DeckTreeTableController.CreateChildDeck(DeckName, ParentDeckId);
    }

    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString& DeckId, const QString& NewParentDeckId = QString{}) noexcept {
        return m_DeckTreeTableController.MoveDeck(DeckId, NewParentDeckId);
    }

    [[nodiscard]] Q_INVOKABLE QString UpdateDeckName(const QString& DeckId, const QString& NewDeckName) noexcept {
        return m_DeckTreeTableController.UpdateDeckName(DeckId, NewDeckName);
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
