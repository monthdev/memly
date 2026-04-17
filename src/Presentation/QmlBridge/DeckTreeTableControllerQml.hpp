#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>

#include "App/Bootstrap/AppContext.hpp"
#include "Controller/DeckTreeTableController.hpp"

namespace Qml {

class DeckTreeTableControllerQml : public QObject {
    Q_OBJECT
    Q_PROPERTY(Model::DeckTreeTableModel* deckTree READ GetDeckTree CONSTANT)
    QML_NAMED_ELEMENT(DeckTreeTableController)

public:
    explicit DeckTreeTableControllerQml(QObject* Parent = nullptr)
        : QObject{ Parent }
        , m_DeckTreeTableController{ App::AppContext::GetRequiredDeckHierarchyStore() } {
    }

    [[nodiscard]] Model::DeckTreeTableModel* GetDeckTree() noexcept {
        return m_DeckTreeTableController.GetDeckTree();
    }

    [[nodiscard]] Q_INVOKABLE QString GetNameLengthErrorMessage() const {
        return m_DeckTreeTableController.GetNameLengthErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString GetDuplicateNameErrorMessage() const {
        return m_DeckTreeTableController.GetDuplicateNameErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString GetTargetLanguageCodeErrorMessage() const {
        return m_DeckTreeTableController.GetTargetLanguageCodeErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString GetParentDeckErrorMessage() const {
        return m_DeckTreeTableController.GetParentDeckErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString GetCycleDetectionErrorMessage() const {
        return m_DeckTreeTableController.GetCycleDetectionErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString CreateDeck(const QString& DeckName, const quint8 TargetLanguageCode, const QString& ParentDeckId = {}) noexcept {
        return m_DeckTreeTableController.CreateDeck(DeckName, TargetLanguageCode, ParentDeckId);
    }

    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString& DeckId, const QString& ParentDeckId = {}) noexcept {
        return m_DeckTreeTableController.MoveDeck(DeckId, ParentDeckId);
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
