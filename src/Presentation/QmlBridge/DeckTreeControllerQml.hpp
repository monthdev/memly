#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <QObject>

#include "App/Bootstrap/AppContext.hpp"
#include "Controller/DeckTreeController.hpp"

namespace Qml {

class DeckTreeControllerQml : public QObject {
    Q_OBJECT
    Q_PROPERTY(Model::DeckTreeModel* deckTree READ GetDeckTree CONSTANT)
    QML_NAMED_ELEMENT(DeckTreeController)

public:
    explicit DeckTreeControllerQml(QObject* Parent = nullptr) noexcept
        : QObject{ Parent }
        , m_DeckTreeController{ App::AppContext::GetRequiredDeckHierarchyStore() } {
    }

    [[nodiscard]] Model::DeckTreeModel* GetDeckTree() noexcept {
        return m_DeckTreeController.GetDeckTree();
    }

    [[nodiscard]] Q_INVOKABLE QString NameLengthErrorMessage() const {
        return m_DeckTreeController.NameLengthErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString DuplicateNameErrorMessage() const {
        return m_DeckTreeController.DuplicateNameErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString TargetLanguageCodeErrorMessage() const {
        return m_DeckTreeController.TargetLanguageCodeErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString ParentDeckErrorMessage() const {
        return m_DeckTreeController.ParentDeckErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString CycleDetectionErrorMessage() const {
        return m_DeckTreeController.CycleDetectionErrorMessage();
    }

    [[nodiscard]] Q_INVOKABLE QString CreateDeck(const QString& DeckName,
                                                 const quint8 TargetLanguageCode,
                                                 const QString& ParentDeckId = {}) noexcept {
        return m_DeckTreeController.CreateDeck(DeckName, TargetLanguageCode, ParentDeckId);
    }

    [[nodiscard]] Q_INVOKABLE QString MoveDeck(const QString& DeckId,
                                               const QString& ParentDeckId = {}) noexcept {
        return m_DeckTreeController.MoveDeck(DeckId, ParentDeckId);
    }

    [[nodiscard]] Q_INVOKABLE QString UpdateDeckName(const QString& DeckId,
                                                     const QString& DeckName) noexcept {
        return m_DeckTreeController.UpdateDeckName(DeckId, DeckName);
    }

    Q_INVOKABLE void OnActivated() {
        m_DeckTreeController.OnActivated();
    }

    Q_INVOKABLE void OnDeactivated() {
        m_DeckTreeController.OnDeactivated();
    }

    Q_INVOKABLE void DeleteDeck(const QString& DeckId) noexcept {
        m_DeckTreeController.DeleteDeck(DeckId);
    }

private:
    Controller::DeckTreeController m_DeckTreeController;
};

}
